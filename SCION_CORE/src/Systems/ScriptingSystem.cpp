#include "Core/Systems/ScriptingSystem.h"
#include "Core/ECS/Components/ScriptComponent.h"
#include "Core/ECS/Components/TransformComponent.h"
#include "Core/ECS/Components/SpriteComponent.h"
#include "Core/ECS/Components/AnimationComponent.h"
#include "Core/ECS/Components/BoxColliderComponent.h"
#include "Core/ECS/Components/CircleColliderComponent.h"
#include "Core/ECS/Components/PhysicsComponent.h"
#include "Core/ECS/Components/TextComponent.h"
#include "Core/ECS/Components/RigidBodyComponent.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/MainRegistry.h"

#include "Core/Scripting/GlmLuaBindings.h"
#include "Core/Scripting/InputManager.h"
#include "Core/Scripting/SoundBindings.h"
#include "Core/Scripting/RendererBindings.h"
#include "Core/Scripting/UserDataBindings.h"
#include "Core/Scripting/ContactListenerBind.h"

#include "Core/Resources/AssetManager.h"
#include <Logger/Logger.h>
#include <ScionUtilities/Timer.h>
#include <ScionUtilities/RandomGenerator.h>
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/CoreUtilities/FollowCamera.h"

#include "Core/States/State.h"
#include "Core/States/StateStack.h"
#include "Core/States/StateMachine.h"

using namespace SCION_CORE::ECS;
using namespace SCION_RESOURCES;

namespace SCION_CORE::Systems
{

ScriptingSystem::ScriptingSystem( SCION_CORE::ECS::Registry& registry )
	: m_Registry( registry )
	, m_bMainLoaded{ false }
{
}

bool ScriptingSystem::LoadMainScript( sol::state& lua )
{
	try
	{
		auto result = lua.safe_script_file( "./assets/scripts/main.lua" );
	}
	catch ( const sol::error& err )
	{
		SCION_ERROR( "Error loading the main lua script: {0}", err.what() );
		return false;
	}

	sol::table main_lua = lua[ "main" ];
	sol::optional<sol::table> bUpdateExists = main_lua[ 1 ];
	if ( bUpdateExists == sol::nullopt )
	{
		SCION_ERROR( "There is no update function in main.lua" );
		return false;
	}

	sol::table update_script = main_lua[ 1 ];
	sol::function update = update_script[ "update" ];

	sol::optional<sol::table> bRenderExists = main_lua[ 2 ];
	if ( bRenderExists == sol::nullopt )
	{
		SCION_ERROR( "There is no render function in main.lua" );
		return false;
	}

	sol::table render_script = main_lua[ 2 ];
	sol::function render = render_script[ "render" ];

	SCION_CORE::ECS::Entity mainLuaScript{ m_Registry, "main_script", "" };
	mainLuaScript.AddComponent<SCION_CORE::ECS::ScriptComponent>(
		SCION_CORE::ECS::ScriptComponent{ .update = update, .render = render } );

	m_bMainLoaded = true;
	return true;
}

void ScriptingSystem::Update()
{
	if ( !m_bMainLoaded )
	{
		SCION_ERROR( "Main lua script has not been loaded!" );
		return;
	}

	auto view = m_Registry.GetRegistry().view<SCION_CORE::ECS::ScriptComponent>();

	for ( const auto& entity : view )
	{
		SCION_CORE::ECS::Entity ent{ m_Registry, entity };
		if ( ent.GetName() != "main_script" )
			continue;

		auto& script = ent.GetComponent<SCION_CORE::ECS::ScriptComponent>();
		auto error = script.update( entity );
		if ( !error.valid() )
		{
			sol::error err = error;
			SCION_ERROR( "Error running the Update script: {0}", err.what() );
		}
	}

	auto& lua = m_Registry.GetContext<std::shared_ptr<sol::state>>();
	if (lua) lua->collect_garbage();
}

void ScriptingSystem::Render()
{
	if ( !m_bMainLoaded )
	{
		SCION_ERROR( "Main lua script has not been loaded!" );
		return;
	}

	auto view = m_Registry.GetRegistry().view<SCION_CORE::ECS::ScriptComponent>();

	for ( const auto& entity : view )
	{
		SCION_CORE::ECS::Entity ent{ m_Registry, entity };
		if ( ent.GetName() != "main_script" )
			continue;

		auto& script = ent.GetComponent<SCION_CORE::ECS::ScriptComponent>();
		auto error = script.render( entity );
		if ( !error.valid() )
		{
			sol::error err = error;
			SCION_ERROR( "Error running the Render script: {0}", err.what() );
		}
	}

	auto& lua = m_Registry.GetContext<std::shared_ptr<sol::state>>();
	if (lua) lua->collect_garbage();
}

auto create_timer = []( sol::state& lua ) {
	using namespace SCION_UTIL;
	lua.new_usertype<Timer>( "Timer",
							 sol::call_constructor,
							 sol::constructors<Timer()>(),
							 "start",
							 &Timer::Start,
							 "stop",
							 &Timer::Stop,
							 "pause",
							 &Timer::Pause,
							 "resume",
							 &Timer::Resume,
							 "is_paused",
							 &Timer::IsPaused,
							 "is_running",
							 &Timer::IsRunning,
							 "elapsed_ms",
							 &Timer::ElapsedMS,
							 "elapsed_sec",
							 &Timer::ElapsedSec,
							 "restart",
							 []( Timer& timer ) {
								 if ( timer.IsRunning() )
									 timer.Stop();

								 timer.Start();
							 } );
};

auto create_lua_logger = [ ]( sol::state& lua ) {
	auto& logger = SCION_LOGGER::Logger::GetInstance();

	lua.new_usertype<SCION_LOGGER::Logger>(
		"Logger",
		sol::no_constructor,
		"log",
		[ & ]( const std::string_view message ) { logger.LuaLog( message ); },
		"warn",
		[ & ]( const std::string_view message ) { logger.LuaWarn( message ); },
		"error",
		[ & ]( const std::string_view message ) { logger.LuaError( message ); } );

	auto logResult = lua.safe_script( R"(
				function ZZZ_Log(message, ...)
					Logger.log(string.format(message, ...))
				end
			)" );

	if ( !logResult.valid() )
	{
		SCION_ERROR( "Failed to initialize lua logs" );
	}

	auto warnResult = lua.safe_script( R"(
				function ZZZ_Warn(message, ...)
					Logger.warn(string.format(message, ...))
				end
			)" );

	if ( !warnResult.valid() )
	{
		SCION_ERROR( "Failed to initialize lua warnings" );
	}

	auto errorResult = lua.safe_script( R"(
				function ZZZ_Error(message, ...)
					Logger.error(string.format(message, ...))
				end
			)" );

	if ( !errorResult.valid() )
	{
		SCION_ERROR( "Failed to initialize lua errors" );
	}

	lua.set_function( "S2D_log", []( const std::string& message, const sol::variadic_args& args, sol::this_state s ) {
		try
		{
			sol::state_view L = s;
			sol::protected_function log = L[ "ZZZ_Log" ];
			auto result = log( message, args );
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}
		catch ( const sol::error& error )
		{
			SCION_ERROR( "Failed to get lua log: {}", error.what() );
		}
	} );

	lua.set_function( "S2D_warn", []( const std::string& message, const sol::variadic_args& args, sol::this_state s ) {
		try
		{
			sol::state_view L = s;
			sol::protected_function warn = L[ "ZZZ_Warn" ];
			auto result = warn( message, args );
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}
		catch ( const sol::error& error )
		{
			SCION_ERROR( "Failed to get lua warning: {}", error.what() );
		}
	} );

	lua.set_function( "S2D_error", []( const std::string& message, const sol::variadic_args& args, sol::this_state s ) {
		try
		{
			sol::state_view L = s;
			sol::protected_function err = L[ "ZZZ_Error" ];
			auto result = err( message, args );
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}
		catch ( const sol::error& error )
		{
			SCION_ERROR( "Failed to get lua errors: {}", error.what() );
		}
	} );

	auto assertResult = lua.safe_script( R"(
				S2D_assert = assert
				assert = function(arg1, message, ...)
					if not arg1 then 
						Logger.error(string.format(message, ...))
					end 
					S2D_assert(arg1)
				end
			)" );
};

void ScriptingSystem::RegisterLuaBindings( sol::state& lua, SCION_CORE::ECS::Registry& registry )
{
	SCION_CORE::Scripting::GLMBindings::CreateGLMBindings( lua );
	SCION_CORE::InputManager::CreateLuaInputBindings( lua, registry );
	SCION_RESOURCES::AssetManager::CreateLuaAssetManager( lua );
	SCION_CORE::Scripting::SoundBinder::CreateSoundBind( lua );
	SCION_CORE::Scripting::RendererBinder::CreateRenderingBind( lua, registry );
	SCION_CORE::Scripting::UserDataBinder::CreateLuaUserData( lua );
	SCION_CORE::Scripting::ContactListenerBinder::CreateLuaContactListener( lua, registry.GetRegistry() );

	SCION_CORE::FollowCamera::CreateLuaFollowCamera( lua, registry );

	create_timer( lua );
	create_lua_logger( lua );

	SCION_CORE::State::CreateLuaStateBind( lua );
	SCION_CORE::StateStack::CreateLuaStateStackBind( lua );
	SCION_CORE::StateMachine::CreateLuaStateMachine( lua );

	Registry::CreateLuaRegistryBind( lua, registry );
	Entity::CreateLuaEntityBind( lua, registry );
	TransformComponent::CreateLuaTransformBind( lua );
	SpriteComponent::CreateSpriteLuaBind( lua );
	AnimationComponent::CreateAnimationLuaBind( lua );
	BoxColliderComponent::CreateLuaBoxColliderBind( lua );
	CircleColliderComponent::CreateLuaCircleColliderBind( lua );
	PhysicsComponent::CreatePhysicsLuaBind( lua, registry.GetRegistry() );
	TextComponent::CreateLuaTextBindings( lua );
	RigidBodyComponent::CreateRigidBodyBind( lua );
}

void ScriptingSystem::RegisterLuaFunctions( sol::state& lua, SCION_CORE::ECS::Registry& registry )
{
	auto& mainRegistry = MAIN_REGISTRY();

	lua.set_function( "S2D_run_script", [ & ]( const std::string& path ) {
		try
		{
			lua.safe_script_file( path );
		}
		catch ( const sol::error& error )
		{
			SCION_ERROR( "Error loading Lua Script: {}", error.what() );
			return false;
		}

		return true;
	} );

	lua.set_function( "S2D_load_script_table", [ & ]( const sol::table& scriptList ) {
		if ( !scriptList.valid() )
		{
			SCION_ERROR( "Failed to load script list: Table is invalid." );
			return;
		}

		for ( const auto& [ index, script ] : scriptList )
		{
			try
			{
				auto result = lua.safe_script_file( script.as<std::string>() );
				if ( !result.valid() )
				{
					sol::error error = result;
					throw error;
				}
			}
			catch ( const sol::error& error )
			{
				SCION_ERROR( "Failed to load script: {}, Error: {}", script.as<std::string>(), error.what() );
				return;
			}
		}
	} );

	lua.set_function( "S2D_get_ticks", [] { return SDL_GetTicks(); } );

	auto& assetManager = mainRegistry.GetAssetManager();
	lua.set_function( "S2D_measure_text", [ & ]( const std::string& text, const std::string& fontName ) {
		const auto& pFont = assetManager.GetFont( fontName );
		if ( !pFont )
		{
			SCION_ERROR( "Failed to get font [{}] - Does not exist in asset manager!", fontName );
			return -1.f;
		}

		glm::vec2 position{ 0.f }, temp_pos{ position };
		for ( const auto& character : text )
			pFont->GetNextCharPos( character, temp_pos );

		return std::abs( ( position - temp_pos ).x );
	} );

	auto& engine = CoreEngineData::GetInstance();
	lua.set_function( "GetDeltaTime", [ & ] { return engine.GetDeltaTime(); } );
	lua.set_function( "WindowWidth", [ & ] { return engine.WindowWidth(); } );
	lua.set_function( "WindowHeight", [ & ] { return engine.WindowHeight(); } );

	// Physics Enable functions
	lua.set_function( "DisablePhysics", [ & ] { engine.DisablePhysics(); } );
	lua.set_function( "EnablePhysics", [ & ] { engine.EnablePhysics(); } );
	lua.set_function( "IsPhysicsEnabled", [ & ] { return engine.IsPhysicsEnabled(); } );

	// Render Colliders Enable functions
	lua.set_function( "DisableRenderColliders", [ & ] { engine.DisableColliderRender(); } );
	lua.set_function( "EnableRenderColliders", [ & ] { engine.EnableColliderRender(); } );
	lua.set_function( "IsRenderCollidersEnabled", [ & ] { return engine.RenderCollidersEnabled(); } );

	lua.new_usertype<SCION_UTIL::RandomGenerator>(
		"Random",
		sol::call_constructor,
		sol::constructors<SCION_UTIL::RandomGenerator( uint32_t, uint32_t ), SCION_UTIL::RandomGenerator()>(),
		"get_float",
		&SCION_UTIL::RandomGenerator::GetFloat,
		"get_int",
		&SCION_UTIL::RandomGenerator::GetInt );

	lua.set_function( "S2D_EntityInView", [ & ]( const TransformComponent& transform, float width, float height ) {
		auto& camera = registry.GetContext<std::shared_ptr<SCION_RENDERING::Camera2D>>();
		return SCION_CORE::EntityInView( transform, width, height, *camera );
	} );
}

} // namespace SCION_CORE::Systems
