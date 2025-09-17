#include "Runtime.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/CoreUtilities/EngineShaders.h"
#include "Core/Resources/AssetManager.h"
#include "Core/Events/EventDispatcher.h"
#include "Core/Events/EngineEventTypes.h"
#include "Core/Scripting/InputManager.h"
#include "Core/Scripting/CrashLoggerTestBindings.h"
#include "Core/Scripting/ScriptingUtilities.h"

#include "Core/Scene/SceneManager.h"

#include "Core/Systems/AnimationSystem.h"
#include "Core/Systems/PhysicsSystem.h"
#include "Core/Systems/ScriptingSystem.h"
#include "Core/Systems/RenderSystem.h"
#include "Core/Systems/RenderUISystem.h"
#include "Core/Systems/RenderShapeSystem.h"

#include "Physics/Box2DWrappers.h"
#include "Physics/ContactListener.h"

#include "Logger/Logger.h"
#include "Logger/CrashLogger.h"
#include "ScionUtilities/HelperUtilities.h"
#include "ScionUtilities/ScionUtilities.h"

#include "Windowing/Window/Window.h"
#include "Windowing/Inputs/Mouse.h"
#include "Windowing/Inputs/Keyboard.h"
#include "Windowing/Inputs/Gamepad.h"
#include "Rendering/Core/Camera2D.h"
#include "Rendering/Core/Renderer.h"

#include "Core/Loaders/TilemapLoader.h"
#include "Core/CoreUtilities/ProjectInfo.h"

#include <SDL2/SDL.h>
#include <sol/sol.hpp>
#include <glad/glad.h>
#include <libzippp/libzippp.h>

namespace fs = std::filesystem;

using namespace SCION_CORE::Systems;
using namespace SCION_CORE::ECS;
using namespace SCION_RENDERING;

namespace SCION_ENGINE
{
RuntimeApp::RuntimeApp()
	: m_pWindow{ nullptr }
	, m_Event{}
	, m_bRunning{ true }
	, m_pGameConfig{ std::make_unique<SCION_CORE::GameConfig>() }
{
}

RuntimeApp::~RuntimeApp()
{
}

void RuntimeApp::Run()
{
	Initialize();

	while ( m_bRunning )
	{
		ProcessEvents();
		Update();
		Render();
	}

	CleanUp();
}

void RuntimeApp::Initialize()
{
	SCION_INIT_LOGS( true, false );
	SCION_INIT_CRASH_LOGS();

	// Init SDL
	if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
	{
		std::string error = SDL_GetError();
		throw std::runtime_error( fmt::format( "Failed to initialize SDL: {}", error ) );
	}

	// Set up OpenGL
	if ( SDL_GL_LoadLibrary( NULL ) != 0 )
	{
		std::string error = SDL_GetError();
		throw std::runtime_error( fmt::format( "Failed to Open GL Library: {}", error ) );
	}

	// Set the OpenGL attributes
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	// Set the number of bits per channel
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );

	auto pLuaState = std::make_shared<sol::state>();
	pLuaState->open_libraries( sol::lib::base,
							   sol::lib::math,
							   sol::lib::os,
							   sol::lib::table,
							   sol::lib::io,
							   sol::lib::string,
							   sol::lib::package,
							   sol::lib::coroutine );

	if ( !pLuaState || !LoadConfig( *pLuaState ) )
	{
		throw std::runtime_error( "Failed to initialize the game configuration." );
	}

	// Set the lua state for the crash logger.
	// This is used to log the lua stack trace in case of a crash
	SCION_CRASH_LOGGER().SetLuaState( pLuaState->lua_state() );

	// Setup Crash Tests
	SCION_CORE::Scripting::CrashLoggerTests::CreateLuaBind( *pLuaState );

	auto& coreGlobals = CORE_GLOBALS();

	// Create the Window
	m_pWindow = std::make_unique<SCION_WINDOWING::Window>( m_pGameConfig->sGameName.c_str(),
														   m_pGameConfig->windowWidth,
														   m_pGameConfig->windowHeight,
														   SDL_WINDOWPOS_CENTERED,
														   SDL_WINDOWPOS_CENTERED,
														   true,
														   m_pGameConfig->windowFlags | SDL_WINDOW_OPENGL |
															   SDL_WINDOW_ALLOW_HIGHDPI );

	// Create the openGL context
	m_pWindow->SetGLContext( SDL_GL_CreateContext( m_pWindow->GetWindow().get() ) );

	// Initialize Glad
	if ( gladLoadGLLoader( SDL_GL_GetProcAddress ) == 0 )
	{
		throw std::runtime_error( "Failed to GLAD" );
	}

	if ( !m_pWindow->GetGLContext() )
	{
		std::string error = SDL_GetError();
		throw std::runtime_error( fmt::format( "Failed to create OpenGL context: {}", error ) );
	}

	if ( ( SDL_GL_MakeCurrent( m_pWindow->GetWindow().get(), m_pWindow->GetGLContext() ) ) != 0 )
	{
		std::string error = SDL_GetError();
		throw std::runtime_error( fmt::format( "Failed to make OpenGL context current: {}", error ) );
	}

	SDL_GL_SetSwapInterval( 1 );

	auto& mainRegistry = MAIN_REGISTRY();
	if ( !mainRegistry.Initialize() )
	{
		throw std::runtime_error( "Failed to initialize the Main Registry." );
	}

	// Allocate SDL_Mixer channels
	if ( m_DeltaAllocatedChannels != 0 && !m_pGameConfig->audioConfig.UpdateSoundChannels( m_DeltaAllocatedChannels ) )
	{
		throw std::runtime_error( "Failed to allocated new channels" );
	}

	if ( !mainRegistry.GetAssetManager().CreateDefaultFonts() )
	{
		throw std::runtime_error( "Failed to create default fonts." );
	}

	mainRegistry.AddToContext<std::shared_ptr<sol::state>>( std::move( pLuaState ) );
	auto mainScript = mainRegistry.AddToContext<MainScriptPtr>( std::make_shared<SCION_CORE::Scripting::MainScriptFunctions>() );
	if ( !LoadShaders() )
	{
		throw std::runtime_error( "Failed to Load game shaders." );
	}

	LoadRegistryContext();
	LoadBindings();
	SCION_CORE::CoreEngineData::RegisterMetaFunctions();

	if ( m_pGameConfig->bPackageAssets && !LoadZip() )
	{
		throw std::runtime_error( "Failed to load game assets zip file." );
	}

	if ( !LoadScripts() )
	{
		throw std::runtime_error( "Failed to load game scripts. " );
	}

	auto pSceneManagerData = mainRegistry.AddToContext<std::shared_ptr<SCION_CORE::SceneManagerData>>(
		std::make_shared<SCION_CORE::SceneManagerData>() );

	SCION_CORE::Loaders::TilemapLoader tl{};
	auto& lua = mainRegistry.GetContext<std::shared_ptr<sol::state>>();
	tl.LoadTilemapFromLuaTable( *mainRegistry.GetRegistry(), ( *lua )[ m_pGameConfig->sStartupScene + "_tilemap" ] );
	tl.LoadGameObjectsFromLuaTable( *mainRegistry.GetRegistry(),
									( *lua )[ m_pGameConfig->sStartupScene + "_objects" ] );

	pSceneManagerData->sSceneName = m_pGameConfig->sStartupScene;

	if ( !mainScript->init.valid() )
	{
		throw std::runtime_error( "Failed to initialize main script. init() function is invalid." );
	}

	mainScript->init();

	if ( coreGlobals.IsPhysicsEnabled() )
	{
		LoadPhysics();
	}
}

bool RuntimeApp::LoadShaders()
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	if ( !assetManager.AddShaderFromMemory(
			 "basic", SCION_CORE::Shaders::basicShaderVert, SCION_CORE::Shaders::basicShaderFrag ) )
	{
		SCION_ERROR( "Failed to add the basic shader to the asset manager" );
		return false;
	}

	if ( !assetManager.AddShaderFromMemory(
			 "color", SCION_CORE::Shaders::colorShaderVert, SCION_CORE::Shaders::colorShaderFrag ) )
	{
		SCION_ERROR( "Failed to add the color shader to the asset manager" );
		return false;
	}

	if ( !assetManager.AddShaderFromMemory(
			 "circle", SCION_CORE::Shaders::circleShaderVert, SCION_CORE::Shaders::circleShaderFrag ) )
	{
		SCION_ERROR( "Failed to add the color shader to the asset manager" );
		return false;
	}

	if ( !assetManager.AddShaderFromMemory(
			 "font", SCION_CORE::Shaders::fontShaderVert, SCION_CORE::Shaders::fontShaderFrag ) )
	{
		SCION_ERROR( "Failed to add the font shader to the asset manager" );
		return false;
	}

	return true;
}

bool RuntimeApp::LoadConfig( sol::state& lua )
{
	auto& coreGlobals = CORE_GLOBALS();
	const auto sConfigFile = fmt::format( "config{}config.luac", PATH_SEPARATOR );
	if ( !fs::exists( fs::path{ sConfigFile } ) )
	{
		SCION_ERROR( "Failed to load configuration. [{}] config file does not exist.", sConfigFile );
		return false;
	}

	try
	{
		lua.safe_script_file( sConfigFile );
	}
	catch ( const sol::error& err )
	{
		throw std::runtime_error( fmt::format( "Failed to load configuration: {}", err.what() ) );
	}

	sol::optional<sol::table> maybeConfig = lua[ "GameConfig" ];
	if ( !maybeConfig )
	{
		throw std::runtime_error( "Game config file is missing the \"GameConfig\" table." );
	}

	const std::string sGameName = ( *maybeConfig )[ "GameName" ].get_or( std::string{ "GAME NAME NOT PROVIDED." } );
	m_pGameConfig->sGameName = sGameName;

	const std::string sStartScene = ( *maybeConfig )[ "StartupScene" ].get_or( std::string{ "" } );

	if ( sStartScene.empty() )
	{
		throw std::runtime_error( "Game config file is missing the \"StartScene\" entry." );
	}

	m_pGameConfig->sStartupScene = sStartScene;

	sol::optional<sol::table> maybeWindow = ( *maybeConfig )[ "WindowParams" ];
	if ( !maybeWindow )
	{
		throw std::runtime_error( "Game config file is missing the \"WindowParams\" table." );
	}

	int windowWidth = ( *maybeWindow )[ "width" ].get_or( 640 );
	int windowHeight = ( *maybeWindow )[ "height" ].get_or( 480 );
	uint32_t windowFlags = ( *maybeWindow )[ "flags" ].get_or( 0U );
	coreGlobals.SetWindowWidth( windowWidth );
	coreGlobals.SetWindowHeight( windowHeight );
	m_pGameConfig->windowWidth = windowWidth;
	m_pGameConfig->windowHeight = windowHeight;
	m_pGameConfig->windowFlags = windowFlags;

	sol::optional<sol::table> maybePhysics = ( *maybeConfig )[ "PhysicsParams" ];
	if ( maybePhysics )
	{
		bool bPhysicsEnabled = ( *maybePhysics )[ "bEnabled" ].get_or( false );
		int32_t positionIterations = ( *maybePhysics )[ "positionIterations" ].get_or( 0 );
		int32_t velocityIterations = ( *maybePhysics )[ "velocityIterations" ].get_or( 0 );
		float gravity = ( *maybePhysics )[ "gravity" ].get_or( 9.8f );
		bPhysicsEnabled ? coreGlobals.EnablePhysics() : coreGlobals.DisablePhysics();

		coreGlobals.SetPositionIterations( positionIterations );
		coreGlobals.SetVelocityIterations( velocityIterations );
		coreGlobals.SetGravity( gravity );
	}

	// TODO: Flags, etc

	m_pGameConfig->bPackageAssets = ( *maybeConfig )[ "bPackageAssets" ].get_or( false );

	sol::optional<sol::table> maybeAudio = ( *maybeConfig )[ "AudioParams" ];
	if (maybeAudio)
	{
		if ((*maybeAudio)["allocatedChannels"].valid())
		{
			int allocatedChannels = m_pGameConfig->audioConfig.GetAllocatedChannelCount();
			int newAllocatedChannels = ( *maybeAudio )[ "allocatedChannels" ].get_or( 0 );
			m_DeltaAllocatedChannels = newAllocatedChannels - allocatedChannels;
		}
	}

	return true;
}

bool RuntimeApp::LoadRegistryContext()
{
	auto& coreGlobals = CORE_GLOBALS();
	auto& mainRegistry = MAIN_REGISTRY();
	// Right now we will make the camera use the window width/height; however, we should add a
	// way to change this through the game config tile.
	mainRegistry.AddToContext<std::shared_ptr<Camera2D>>(
		std::make_shared<Camera2D>( coreGlobals.WindowWidth(), coreGlobals.WindowHeight() ) );

	if ( coreGlobals.IsPhysicsEnabled() )
	{
		auto pPhysicsWorld = mainRegistry.AddToContext<SCION_PHYSICS::PhysicsWorld>(
			std::make_shared<b2World>( b2Vec2{ 0.f, coreGlobals.GetGravity() } ) );

		auto pContactListener = mainRegistry.AddToContext<std::shared_ptr<SCION_PHYSICS::ContactListener>>(
			std::make_shared<SCION_PHYSICS::ContactListener>() );

		pPhysicsWorld->SetContactListener( pContactListener.get() );
	}

	mainRegistry.AddToContext<std::shared_ptr<ScriptingSystem>>( std::make_shared<ScriptingSystem>() );

	return false;
}

void RuntimeApp::LoadBindings()
{
	auto& mainRegistry = MAIN_REGISTRY();

	auto& pLuaState = mainRegistry.GetContext<std::shared_ptr<sol::state>>();
	auto* pRegistry = mainRegistry.GetRegistry();
	
	ScriptingSystem::RegisterLuaBindings( *pLuaState, *pRegistry );
	ScriptingSystem::RegisterLuaFunctions( *pLuaState, *pRegistry );
	ScriptingSystem::RegisterLuaEvents( *pLuaState, *pRegistry );
	ScriptingSystem::RegisterLuaSystems( *pLuaState, *pRegistry );

	SCION_CORE::SceneManager::CreateLuaBind( *pLuaState, *pRegistry );
}

bool RuntimeApp::LoadScripts()
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& scriptSystem = mainRegistry.GetContext<std::shared_ptr<ScriptingSystem>>();
	auto& lua = mainRegistry.GetContext<std::shared_ptr<sol::state>>();

	return scriptSystem->LoadMainScript( "assets/scripts/master.luac", *mainRegistry.GetRegistry(), *lua );
}

bool RuntimeApp::LoadPhysics()
{
	auto& coreGlobals = CORE_GLOBALS();
	auto& mainRegistry = MAIN_REGISTRY();
	auto* pRegistry = mainRegistry.GetRegistry();
	auto& pPhysicsWorld = mainRegistry.GetContext<SCION_PHYSICS::PhysicsWorld>();
	auto& pCamera = mainRegistry.GetContext<std::shared_ptr<Camera2D>>();

	// We need to initialize all of the physics entities
	auto physicsEntities = pRegistry->GetRegistry().view<PhysicsComponent>();
	for ( auto entity : physicsEntities )
	{
		Entity ent{ pRegistry, entity };

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

	return true;
}

bool RuntimeApp::LoadZip()
{
	auto& assetManager = MAIN_REGISTRY().GetAssetManager();
	auto& coreGlobals = CORE_GLOBALS();

	const std::string zipAssetsPath{ fmt::format( "{}{}{}", "assets", PATH_SEPARATOR, "ScionAssets.zip" ) };

	if ( !fs::exists( fs::path{ zipAssetsPath } ) )
	{
		throw std::runtime_error( fmt::format( "Failed to load zipped assets at path: {}", zipAssetsPath ) );
	}

	libzippp::ZipArchive zipArchive{ zipAssetsPath };
	zipArchive.open( libzippp::ZipArchive::ReadOnly );
	std::vector<libzippp::ZipEntry> entries = zipArchive.getEntries();

	using namespace SCION_UTIL;

	for ( const auto& entry : entries )
	{
		auto text = entry.readAsText();
		sol::state lua;
		try
		{
			auto result = lua.safe_script( text );
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}
		catch ( const sol::error& error )
		{
			SCION_ERROR( "Failed to read in assets! {}", error.what() );
		}

		sol::optional<sol::table> s2dAsset = lua[ "S2D_Assets" ];
		if ( s2dAsset )
		{
			for ( const auto& [ index, assetTable ] : *s2dAsset )
			{
				sol::table asset = assetTable.as<sol::table>();
				auto pS2DAsset = std::make_unique<S2DAsset>();
				pS2DAsset->sName = asset[ "assetName" ].get_or( std::string{ "" } );
				pS2DAsset->eType = StrToAssetType( asset[ "assetType" ].get_or( std::string{ "" } ) );
				pS2DAsset->assetSize = asset[ "dataSize" ].get_or( 0U );
				pS2DAsset->assetEnd = asset[ "dataEnd" ].get_or( 0U );

				if ( pS2DAsset->eType == SCION_UTIL::AssetType::FONT )
				{
					pS2DAsset->optFontSize = asset[ "fontSize" ].get_or( 32.f );
				}
				else if ( pS2DAsset->eType == SCION_UTIL::AssetType::TEXTURE )
				{
					pS2DAsset->optPixelArt = asset[ "bPixelArt" ].get_or( false );
				}

				// Get the asset data
				sol::table dataTable = asset[ "data" ];
				for ( const auto& [ _, data ] : dataTable )
				{
					auto value = data.as<unsigned char>();
					pS2DAsset->assetData.push_back( value );
				}

				m_mapS2DAssets[ pS2DAsset->eType ].push_back( std::move( pS2DAsset ) );
			}
		}
	}

	for ( const auto& [ eType, assets ] : m_mapS2DAssets )
	{
		switch ( eType )
		{
		case AssetType::TEXTURE: {
			for ( const auto& pTexAsset : assets )
			{
				if ( !assetManager.AddTextureFromMemory( pTexAsset->sName,
														 pTexAsset->assetData.data(),
														 pTexAsset->assetSize,
														 ( pTexAsset->optPixelArt ? *pTexAsset->optPixelArt : true ) ) )
				{
					SCION_ERROR( "Failed to add texture [{}] from memory.", pTexAsset->sName );
				}
			}
			break;
		}
		case AssetType::MUSIC: {
			for ( const auto& pMusicAsset : assets )
			{
				if ( !assetManager.AddMusicFromMemory(
						 pMusicAsset->sName, pMusicAsset->assetData.data(), pMusicAsset->assetSize ) )
				{
					SCION_ERROR( "Failed to add music [{}] from memory.", pMusicAsset->sName );
				}
			}
			break;
		}
		case AssetType::SOUNDFX: {
			for ( const auto& pSoundfxAsset : assets )
			{
				if ( !assetManager.AddSoundFxFromMemory(
						 pSoundfxAsset->sName, pSoundfxAsset->assetData.data(), pSoundfxAsset->assetSize ) )
				{
					SCION_ERROR( "Failed to add music [{}] from memory.", pSoundfxAsset->sName );
				}
			}
			break;
		}
		case AssetType::FONT: {
			for ( const auto& pFontAsset : assets )
			{
				if ( !assetManager.AddFontFromMemory( pFontAsset->sName,
													  pFontAsset->assetData.data(),
													  ( pFontAsset->optFontSize ? *pFontAsset->optFontSize : 32.f ) ) )
				{
					SCION_ERROR( "Failed to add font [{}] from memory.", pFontAsset->sName );
				}
			}
			break;
		}
		}
	}

	zipArchive.close();
	return true;
}

void RuntimeApp::ProcessEvents()
{
	auto& inputManager = INPUT_MANAGER();
	auto& keyboard = inputManager.GetKeyboard();
	auto& mouse = inputManager.GetMouse();

	// Process Events
	while ( SDL_PollEvent( &m_Event ) )
	{
		switch ( m_Event.type )
		{
		case SDL_QUIT: m_bRunning = false; break;
		case SDL_KEYDOWN:
			keyboard.OnKeyPressed( m_Event.key.keysym.sym );
			EVENT_DISPATCHER().EmitEvent( SCION_CORE::Events::KeyEvent{
				.key = m_Event.key.keysym.sym, .eType = SCION_CORE::Events::EKeyEventType::Pressed } );
			break;
		case SDL_KEYUP:
			keyboard.OnKeyReleased( m_Event.key.keysym.sym );
			EVENT_DISPATCHER().EmitEvent( SCION_CORE::Events::KeyEvent{
				.key = m_Event.key.keysym.sym, .eType = SCION_CORE::Events::EKeyEventType::Released } );
			break;
		case SDL_MOUSEBUTTONDOWN: mouse.OnBtnPressed( m_Event.button.button ); break;
		case SDL_MOUSEBUTTONUP: mouse.OnBtnReleased( m_Event.button.button ); break;
		case SDL_MOUSEWHEEL:
			mouse.SetMouseWheelX( m_Event.wheel.x );
			mouse.SetMouseWheelY( m_Event.wheel.y );
			break;
		case SDL_MOUSEMOTION: mouse.SetMouseMoving( true ); break;
		case SDL_CONTROLLERBUTTONDOWN: inputManager.GamepadBtnPressed( m_Event ); break;
		case SDL_CONTROLLERBUTTONUP: inputManager.GamepadBtnReleased( m_Event ); break;
		case SDL_CONTROLLERDEVICEADDED: {
			int index = inputManager.AddGamepad( m_Event.jdevice.which );
			if ( index > 0 )
			{
				EVENT_DISPATCHER().EmitEvent( SCION_CORE::Events::GamepadConnectEvent{
					.eConnectType = SCION_CORE::Events::EGamepadConnectType::Connected, .index = index } );
			}

			break;
		}
		case SDL_CONTROLLERDEVICEREMOVED: {
			int index = inputManager.RemoveGamepad( m_Event.jdevice.which );

			if ( index > 0 )
			{
				EVENT_DISPATCHER().EmitEvent( SCION_CORE::Events::GamepadConnectEvent{
					.eConnectType = SCION_CORE::Events::EGamepadConnectType::Disconnected, .index = index } );
			}

			break;
		}
		case SDL_JOYAXISMOTION: inputManager.GamepadAxisValues( m_Event ); break;
		case SDL_JOYHATMOTION: inputManager.GamepadHatValues( m_Event ); break;
		case SDL_WINDOWEVENT: {
			switch ( m_Event.window.event )
			{
			case SDL_WINDOWEVENT_SIZE_CHANGED: m_pWindow->SetSize( m_Event.window.data1, m_Event.window.data2 ); break;
			default: break;
			}
			break;
		}
		default: break;
		}
	}
}

void RuntimeApp::Update()
{
	auto& coreGlobals = CORE_GLOBALS();
	auto& mainRegistry = MAIN_REGISTRY();
	auto* registry = mainRegistry.GetRegistry();

	double dt = coreGlobals.GetDeltaTime();
	coreGlobals.UpdateDeltaTime();

	// Clamp delta time to the target frame rate
	if ( dt < SCION_CORE::TARGET_FRAME_TIME )
	{
		std::this_thread::sleep_for( std::chrono::duration<double>( SCION_CORE::TARGET_FRAME_TIME - dt ) );
	}

	auto& scriptSystem = mainRegistry.GetContext<std::shared_ptr<ScriptingSystem>>();
	scriptSystem->Update( *registry );

	if ( coreGlobals.IsPhysicsEnabled() && !coreGlobals.IsPhysicsPaused() )
	{
		auto& pPhysicsWorld = mainRegistry.GetContext<SCION_PHYSICS::PhysicsWorld>();
		pPhysicsWorld->Step(
			SCION_CORE::TARGET_FRAME_TIME_F, coreGlobals.GetVelocityIterations(), coreGlobals.GetPositionIterations() );
		pPhysicsWorld->ClearForces();

		auto& dispatch = mainRegistry.GetContext<std::shared_ptr<SCION_CORE::Events::EventDispatcher>>();

		// If there are no listeners for contact events, don't emit event
		if ( dispatch->HasHandlers<SCION_CORE::Events::ContactEvent>() )
		{
			if ( auto& pContactListener = mainRegistry.GetContext<std::shared_ptr<SCION_PHYSICS::ContactListener>>() )
			{
				auto pUserDataA = pContactListener->GetUserDataA();
				auto pUserDataB = pContactListener->GetUserDataB();

				// Only emit contact event if both contacts are valid
				if ( pUserDataA && pUserDataB )
				{
					try
					{
						auto ObjectA = std::any_cast<SCION_PHYSICS::ObjectData>( pUserDataA->userData );
						auto ObjectB = std::any_cast<SCION_PHYSICS::ObjectData>( pUserDataB->userData );

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

		auto& pPhysicsSystem = mainRegistry.GetPhysicsSystem();
		pPhysicsSystem.Update( *registry );
	}

	auto& camera = mainRegistry.GetContext<std::shared_ptr<Camera2D>>();
	mainRegistry.GetAnimationSystem().Update( *registry, *camera );

#ifdef _DEBUG
	if ( INPUT_MANAGER().GetKeyboard().IsKeyJustPressed( SCION_KEY_F2 ) )
	{
		coreGlobals.ToggleRenderCollisions();
	}
#endif

	INPUT_MANAGER().UpdateInputs();
	camera->Update();
}

void RuntimeApp::Render()
{
	auto& coreGlobals = CORE_GLOBALS();
	auto& mainRegistry = MAIN_REGISTRY();
	auto& renderer = mainRegistry.GetRenderer();
	auto* registry = mainRegistry.GetRegistry();

	int w, h;
	SDL_GetWindowSize( m_pWindow->GetWindow().get(), &w, &h );

	renderer.SetViewport( 0, 0, w, h );
	renderer.SetClearColor( 0.0f, 0.0f, 0.0f, 1.f );
	renderer.ClearBuffers( true, true, false );

	auto& camera = mainRegistry.GetContext<std::shared_ptr<Camera2D>>();
	mainRegistry.GetRenderSystem().Update( *mainRegistry.GetRegistry(), *camera );
	mainRegistry.GetRenderUISystem().Update( *mainRegistry.GetRegistry() );

	if ( coreGlobals.RenderCollidersEnabled() )
	{
		mainRegistry.GetRenderShapeSystem().Update( *mainRegistry.GetRegistry(), *camera );
	}

	auto& scriptSystem = mainRegistry.GetContext<std::shared_ptr<ScriptingSystem>>();
	scriptSystem->Render( *registry );

	SDL_GL_SwapWindow( m_pWindow->GetWindow().get() );
}

void RuntimeApp::CleanUp()
{
	SDL_Quit();
}

} // namespace SCION_ENGINE
