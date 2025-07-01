#include "SceneDisplay.h"
#include "Rendering/Buffers/Framebuffer.h"
#include "Rendering/Core/Camera2D.h"
#include "Rendering/Core/Renderer.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/Systems/AnimationSystem.h"
#include "Core/Systems/RenderSystem.h"
#include "Core/Systems/RenderUISystem.h"
#include "Core/Systems/RenderShapeSystem.h"
#include "Core/Systems/PhysicsSystem.h"
#include "Core/Systems/ScriptingSystem.h"
#include "Core/CoreUtilities/CoreEngineData.h"

#include "Logger/Logger.h"
#include "Logger/CrashLogger.h"

#include "Core/Scripting/CrashLoggerTestBindings.h"

#include "Sounds/MusicPlayer/MusicPlayer.h"
#include "Sounds/SoundPlayer/SoundFxPlayer.h"
#include "Physics/Box2DWrappers.h"
#include "Physics/ContactListener.h"
#include "Core/Resources/AssetManager.h"

#include "editor/utilities/EditorFramebuffers.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "Core/CoreUtilities/SaveProject.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"

#include "editor/scripting/EditorCoreLuaWrappers.h"

#include "Core/Events/EventDispatcher.h"
#include "Core/Events/EngineEventTypes.h"
#include "Windowing/Inputs/Keys.h"

#include <imgui.h>
#include <thread>

using namespace SCION_CORE::Systems;
using namespace SCION_CORE::ECS;
using namespace SCION_RENDERING;
using namespace SCION_PHYSICS;

constexpr float TARGET_FRAME_TIME_F = 1.f / 60.f;
constexpr double TARGET_FRAME_TIME = 1.0 / 60.0;

namespace SCION_EDITOR
{
void SceneDisplay::LoadScene()
{
	auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();
	if ( !pCurrentScene )
		return;

	pCurrentScene->CopySceneToRuntime();
	auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();

	const auto& canvas = pCurrentScene->GetCanvas();
	auto pCamera = runtimeRegistry.AddToContext<std::shared_ptr<Camera2D>>(
		std::make_shared<Camera2D>( canvas.width, canvas.height ) );

	auto pPhysicsWorld = runtimeRegistry.AddToContext<SCION_PHYSICS::PhysicsWorld>(
		std::make_shared<b2World>( b2Vec2{ 0.f, CORE_GLOBALS().GetGravity() } ) );

	auto pContactListener = runtimeRegistry.AddToContext<std::shared_ptr<SCION_PHYSICS::ContactListener>>(
		std::make_shared<SCION_PHYSICS::ContactListener>() );

	pPhysicsWorld->SetContactListener( pContactListener.get() );

	// Add the temporary event dispatcher
	runtimeRegistry.AddToContext<std::shared_ptr<SCION_CORE::Events::EventDispatcher>>(
		std::make_shared<SCION_CORE::Events::EventDispatcher>() );

	// Add necessary systems
	auto scriptSystem =
		runtimeRegistry.AddToContext<std::shared_ptr<ScriptingSystem>>( std::make_shared<ScriptingSystem>() );
	runtimeRegistry.AddToContext<std::shared_ptr<MouseGuiInfo>>( std::make_shared<MouseGuiInfo>() );

	auto lua = runtimeRegistry.AddToContext<std::shared_ptr<sol::state>>( std::make_shared<sol::state>() );

	if ( !lua )
	{
		lua = std::make_shared<sol::state>();
	}

	lua->open_libraries( sol::lib::base,
						 sol::lib::math,
						 sol::lib::os,
						 sol::lib::table,
						 sol::lib::io,
						 sol::lib::string,
						 sol::lib::package );

	SCION_CORE::Systems::ScriptingSystem::RegisterLuaBindings( *lua, runtimeRegistry );
	SCION_CORE::Systems::ScriptingSystem::RegisterLuaFunctions( *lua, runtimeRegistry );
	SCION_CORE::Systems::ScriptingSystem::RegisterLuaEvents( *lua, runtimeRegistry );
	SCION_CORE::Systems::ScriptingSystem::RegisterLuaSystems( *lua, runtimeRegistry );
	LuaCoreBinder::CreateLuaBind( *lua, runtimeRegistry );

	EditorSceneManager::CreateSceneManagerLuaBind( *lua );

	// We need to initialize all of the physics entities
	auto physicsEntities = runtimeRegistry.GetRegistry().view<PhysicsComponent>();
	for ( auto entity : physicsEntities )
	{
		Entity ent{ runtimeRegistry, entity };

		bool bBoxCollider{ ent.HasComponent<BoxColliderComponent>() };
		bool bCircleCollider{ ent.HasComponent<CircleColliderComponent>() };

		if ( !bBoxCollider && !bCircleCollider )
		{
			SCION_ERROR( "Entity must have a box or circle collider component to initialize physics on it." );
			continue;
		}

		auto& physics = ent.GetComponent<PhysicsComponent>();
		auto& physicsAttributes = physics.GetChangableAttributes();

		if ( bBoxCollider )
		{
			const auto& boxCollider = ent.GetComponent<BoxColliderComponent>();
			physicsAttributes.boxSize = glm::vec2{ boxCollider.width, boxCollider.height };
			physicsAttributes.offset = boxCollider.offset;
		}

		else if ( bCircleCollider )
		{
			const auto& circleCollider = ent.GetComponent<CircleColliderComponent>();
			physicsAttributes.radius = circleCollider.radius;
			physicsAttributes.offset = circleCollider.offset;
		}

		const auto& transform = ent.GetComponent<TransformComponent>();
		physicsAttributes.position = transform.position;
		physicsAttributes.scale = transform.scale;
		physicsAttributes.objectData.entityID = static_cast<std::int32_t>( entity );

		physics.Init( pPhysicsWorld, pCamera->GetWidth(), pCamera->GetHeight() );

		/*
		 * Set Filters/Masks/Group Index
		 */
		if ( physics.UseFilters() ) // Right now filters are disabled, since there is no way to set this from the editor
		{
			physics.SetFilterCategory();
			physics.SetFilterMask();

			// Should the group index be set based on the sprite layer?
			physics.SetGroupIndex();
		}
	}

	// Get the main script path
	auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<SCION_CORE::SaveProject>>();

	if ( !scriptSystem->LoadMainScript( *pSaveProject, runtimeRegistry, *lua ) )
	{
		SCION_ERROR( "Failed to load the main lua script!" );
		return;
	}

	// Setup Crash Tests
	SCION_CORE::Scripting::CrashLoggerTests::CreateLuaBind( *lua );

	// Set the lua state for the crash logger.
	// This is used to log the lua stack trace in case of a crash
	SCION_CRASH_LOGGER().SetLuaState( lua->lua_state() );

	m_bSceneLoaded = true;
	m_bPlayScene = true;
}

void SceneDisplay::UnloadScene()
{
	m_bPlayScene = false;
	m_bSceneLoaded = false;
	auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();

	SCION_ASSERT( pCurrentScene && "Current Scene must be Valid." );

	auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();

	runtimeRegistry.ClearRegistry();
	runtimeRegistry.RemoveContext<std::shared_ptr<Camera2D>>();
	runtimeRegistry.RemoveContext<SCION_PHYSICS::PhysicsWorld>();
	runtimeRegistry.RemoveContext<std::shared_ptr<SCION_PHYSICS::ContactListener>>();
	runtimeRegistry.RemoveContext<std::shared_ptr<ScriptingSystem>>();
	runtimeRegistry.RemoveContext<std::shared_ptr<SCION_CORE::Events::EventDispatcher>>();
	runtimeRegistry.RemoveContext<std::shared_ptr<sol::state>>();

	auto& mainRegistry = MAIN_REGISTRY();
	mainRegistry.GetMusicPlayer().Stop();
	mainRegistry.GetSoundPlayer().Stop( -1 );
}

void SceneDisplay::RenderScene() const
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& editorFramebuffers = mainRegistry.GetContext<std::shared_ptr<EditorFramebuffers>>();
	auto& renderer = mainRegistry.GetContext<std::shared_ptr<SCION_RENDERING::Renderer>>();

	auto& renderSystem = mainRegistry.GetRenderSystem();
	auto& renderUISystem = mainRegistry.GetRenderUISystem();
	auto& renderShapeSystem = mainRegistry.GetRenderShapeSystem();

	const auto& fb = editorFramebuffers->mapFramebuffers[ FramebufferType::SCENE ];

	fb->Bind();
	renderer->SetViewport( 0, 0, fb->Width(), fb->Height() );
	renderer->SetClearColor( 0.f, 0.f, 0.f, 1.f );
	renderer->ClearBuffers( true, true, false );

	auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();

	if ( pCurrentScene && m_bPlayScene )
	{
		auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();
		auto& camera = runtimeRegistry.GetContext<std::shared_ptr<Camera2D>>();
		renderSystem.Update( runtimeRegistry, *camera );

		if ( CORE_GLOBALS().RenderCollidersEnabled() )
		{
			renderShapeSystem.Update( runtimeRegistry, *camera );
		}

		renderUISystem.Update( runtimeRegistry );

		// Add Render Script stuff after everything???
		auto& scriptSystem = runtimeRegistry.GetContext<std::shared_ptr<SCION_CORE::Systems::ScriptingSystem>>();
		scriptSystem->Render( runtimeRegistry );
	}

	fb->Unbind();
	fb->CheckResize();
}

void SceneDisplay::HandleKeyEvent( const SCION_CORE::Events::KeyEvent keyEvent )
{
	if ( m_bSceneLoaded )
	{
		if ( keyEvent.eType == SCION_CORE::Events::EKeyEventType::Released )
		{
			if ( keyEvent.key == SCION_KEY_ESCAPE )
			{
				UnloadScene();
			}
		}
	}

	// Send double dispatch events to the scene dispatcher.
	auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();
	if ( !pCurrentScene )
		return;

	auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();

	if ( auto* pEventDispatcher =
			 runtimeRegistry.TryGetContext<std::shared_ptr<SCION_CORE::Events::EventDispatcher>>() )
	{
		if ( !pEventDispatcher->get()->HasHandlers<SCION_CORE::Events::KeyEvent>() )
			return;

		pEventDispatcher->get()->EmitEvent( keyEvent );
	}
}

void SceneDisplay::DrawToolbar()
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	auto pPlayTexture = assetManager.GetTexture( "play_button" );
	auto pStopTexture = assetManager.GetTexture( "stop_button" );

	SCION_ASSERT( pPlayTexture && pStopTexture );

	ImGui::Separator();
	ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, { 0.f, 0.f } );

	auto playTextureID = (ImTextureID)(intptr_t)pPlayTexture->GetID();
	if ( m_bPlayScene && m_bSceneLoaded )
	{
		ImGui::ActiveImageButton( "##playButton", playTextureID );
	}
	else
	{
		if ( ImGui::ImageButton( "##playButton", playTextureID, TOOL_BUTTON_SIZE ) )
		{
			LoadScene();
		}
	}

	if ( ImGui::IsItemHovered( ImGuiHoveredFlags_DelayNormal ) )
		ImGui::SetTooltip( "Play Scene" );

	ImGui::SameLine();

	auto stopTextureID = (ImTextureID)(intptr_t)pStopTexture->GetID();
	if ( !m_bPlayScene && !m_bSceneLoaded )
	{
		ImGui::ActiveImageButton( "##stopButton", stopTextureID );
	}
	else
	{
		if ( ImGui::ImageButton( "##stopButton", stopTextureID, TOOL_BUTTON_SIZE ) )
		{
			UnloadScene();
		}
	}

	if ( ImGui::IsItemHovered( ImGuiHoveredFlags_DelayNormal ) )
		ImGui::SetTooltip( "Stop Scene" );

	ImGui::Separator();
	ImGui::PopStyleVar( 1 );
}

SceneDisplay::SceneDisplay()
	: m_bPlayScene{ false }
	, m_bWindowActive{ false }
	, m_bSceneLoaded{ false }
{
	ADD_EVENT_HANDLER( SCION_CORE::Events::KeyEvent, &SceneDisplay::HandleKeyEvent, *this );
}

void SceneDisplay::Draw()
{
	static bool pOpen{ true };
	if ( !ImGui::Begin( "Scene", &pOpen ) )
	{
		ImGui::End();
		return;
	}

	DrawToolbar();
	RenderScene();

	if ( ImGui::BeginChild(
			 "##SceneChild", ImVec2{ 0.f, 0.f }, ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollWithMouse ) )
	{
		m_bWindowActive = ImGui::IsWindowFocused();

		auto& editorFramebuffers = MAIN_REGISTRY().GetContext<std::shared_ptr<EditorFramebuffers>>();
		const auto& fb = editorFramebuffers->mapFramebuffers[ FramebufferType::SCENE ];

		if ( auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject() )
		{
			auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();
			// We need to set the relative mouse window so that any scripts scripts will
			// take into account the position of the imgui window relative to the actual window
			// position, size, etc.
			if ( auto* pMouseInfo = runtimeRegistry.TryGetContext<std::shared_ptr<MouseGuiInfo>>() )
			{
				ImGuiIO io = ImGui::GetIO();
				auto relativePos = ImGui::GetCursorScreenPos();
				ImVec2 windowSize{ ImGui::GetWindowSize() };

				( *pMouseInfo )->position = glm::vec2{ io.MousePos.x - relativePos.x, io.MousePos.y - relativePos.y };
				( *pMouseInfo )->windowSize = glm::vec2{ fb->Width(), fb->Height() };
			}
		}

		ImGui::Image( (ImTextureID)(intptr_t)fb->GetTextureID(),
					  ImVec2{ static_cast<float>( fb->Width() ), static_cast<float>( fb->Height() ) },
					  ImVec2{ 0.f, 1.f },
					  ImVec2{ 1.f, 0.f } );

		ImGui::EndChild();

		// Check for resize based on the window size
		ImVec2 windowSize{ ImGui::GetWindowSize() };
		if ( fb->Width() != static_cast<int>( windowSize.x ) || fb->Height() != static_cast<int>( windowSize.y ) )
			fb->Resize( static_cast<int>( windowSize.x ), static_cast<int>( windowSize.y ) );
	}

	ImGui::End();
}

void SceneDisplay::Update()
{
	if ( !m_bPlayScene )
		return;

	auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();
	if ( !pCurrentScene )
		return;

	auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();

	auto& mainRegistry = MAIN_REGISTRY();
	auto& coreGlobals = CORE_GLOBALS();

	double dt = coreGlobals.GetDeltaTime();
	coreGlobals.UpdateDeltaTime();

	// Clamp delta time to the target frame rate
	if ( dt < TARGET_FRAME_TIME )
	{
		std::this_thread::sleep_for( std::chrono::duration<double>( TARGET_FRAME_TIME - dt ) );
	}

	auto& camera = runtimeRegistry.GetContext<std::shared_ptr<SCION_RENDERING::Camera2D>>();
	if ( !camera )
	{
		SCION_ERROR( "Failed to get the camera from the registry context!" );
		return;
	}

	camera->Update();

	auto& scriptSystem = runtimeRegistry.GetContext<std::shared_ptr<SCION_CORE::Systems::ScriptingSystem>>();
	scriptSystem->Update( runtimeRegistry );

	if ( coreGlobals.IsPhysicsEnabled() )
	{
		auto& pPhysicsWorld = runtimeRegistry.GetContext<SCION_PHYSICS::PhysicsWorld>();
		pPhysicsWorld->Step(
			TARGET_FRAME_TIME_F, coreGlobals.GetVelocityIterations(), coreGlobals.GetPositionIterations() );
		pPhysicsWorld->ClearForces();

		auto& dispatch = runtimeRegistry.GetContext<std::shared_ptr<SCION_CORE::Events::EventDispatcher>>();

		// If there are no listeners for contact events, don't emit event
		if ( dispatch->HasHandlers<SCION_CORE::Events::ContactEvent>() )
		{
			if ( auto& pContactListener = runtimeRegistry.GetContext<std::shared_ptr<ContactListener>>() )
			{
				auto pUserDataA = pContactListener->GetUserDataA();
				auto pUserDataB = pContactListener->GetUserDataB();

				// Only emit contact event if both contacts are valid
				if ( pUserDataA && pUserDataB )
				{
					try
					{
						auto ObjectA = std::any_cast<ObjectData>( pUserDataA->userData );
						auto ObjectB = std::any_cast<ObjectData>( pUserDataB->userData );

						dispatch->EmitEvent(
							SCION_CORE::Events::ContactEvent{ .objectA = ObjectA, .objectB = ObjectB } );
					}
					catch ( const std::bad_any_cast& e )
					{
						SCION_ERROR( "Failed to cast to object data - {}", e.what() );
					}
				}
			}
		}
	}

	auto& pPhysicsSystem = mainRegistry.GetPhysicsSystem();
	pPhysicsSystem.Update( runtimeRegistry );

	auto& animationSystem = mainRegistry.GetAnimationSystem();
	animationSystem.Update( runtimeRegistry, *camera );
}
} // namespace SCION_EDITOR
