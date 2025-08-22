#include "Application.h"
#include <glad/glad.h>

#include <Rendering/Utils/OpenGLDebugger.h>
#include <Rendering/Core/Renderer.h>
#include <Rendering/Essentials/PickingTexture.h>

#include <Logger/Logger.h>
#include <Logger/CrashLogger.h>
#include <Core/ECS/MainRegistry.h>

#include <Core/Resources/AssetManager.h>
#include <Core/CoreUtilities/CoreEngineData.h>

#include <Core/Scripting/InputManager.h>
#include <Core/CoreUtilities/EngineShaders.h>

#include <Windowing/Inputs/Keyboard.h>
#include <Windowing/Inputs/Mouse.h>
#include <Windowing/Inputs/Gamepad.h>
#include <Windowing/Window/Window.h>

#include "editor/displays/MenuDisplay.h"
#include "editor/displays/AssetDisplay.h"
#include "editor/displays/SceneDisplay.h"
#include "editor/displays/ScriptDisplay.h"
#include "editor/displays/SceneHierarchyDisplay.h"
#include "editor/displays/TileDetailsDisplay.h"
#include "editor/displays/TilesetDisplay.h"
#include "editor/displays/TilemapDisplay.h"
#include "editor/displays/LogDisplay.h"
#include "editor/displays/EditorStyleToolDisplay.h"
#include "editor/displays/ContentDisplay.h"
#include "editor/displays/PackageDisplay.h"
#include "editor/displays/ProjectSettingsDisplay.h"

#include "editor/scene/SceneManager.h"

#include "editor/utilities/editor_textures.h"
#include "editor/utilities/EditorFramebuffers.h"
#include "editor/utilities/DrawComponentUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"

#include "Core/CoreUtilities/ProjectInfo.h"
#include "editor/systems/GridSystem.h"
#include "editor/systems/EditorRenderSystem.h"

#include "editor/events/EditorEventTypes.h"
#include "Core/Events/EventDispatcher.h"
#include "Core/Events/EngineEventTypes.h"

#include "ScionUtilities/ThreadPool.h"
#include "ScionUtilities/HelperUtilities.h"
#include "editor/hub/Hub.h"

// IMGUI
// ===================================
#include <imgui_internal.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL_opengl.h>
#include "editor/utilities/imgui/Gui.h"
// ===================================

namespace SCION_EDITOR
{
bool Application::Initialize()
{
	SCION_INIT_LOGS( false, true );
	SCION_INIT_CRASH_LOGS();

	// Init SDL
	if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
	{
		std::string error = SDL_GetError();
		SCION_ERROR( "Failed to initialize SDL: {0}", error );
		return false;
	}

	// Set up OpenGL
	if ( SDL_GL_LoadLibrary( NULL ) != 0 )
	{
		std::string error = SDL_GetError();
		SCION_ERROR( "Failed to Open GL Library: {0}", error );
		return false;
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

	// Create the Window
	m_pWindow = std::make_unique<SCION_WINDOWING::Window>(
		"SCION 2D", 800, 600, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, true, SDL_WINDOW_OPENGL );

	/*
	 * SDL Hack - If we create the window as borderless, we lose the icon in the title bar.
	 * Therefore, after testing I found that if we create the window with a border, then hide
	 * the border. When we show the border later on, the icon will be there.
	 */
	SDL_SetWindowBordered( m_pWindow->GetWindow().get(), SDL_FALSE );

	if ( !m_pWindow->GetWindow() )
	{
		SCION_ERROR( "Failed to create the window!" );
		return false;
	}

	{ // Configure the openGL context attributes
		int attributes{ 0 };
		SDL_GL_GetAttribute( SDL_GL_CONTEXT_FLAGS, &attributes );

#ifdef SCION_OPENGL_DEBUG_CALLBACK
		attributes |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif //! SCION_OPENGL_DEBUG_CALLBACK

#ifdef SCION_OPENGL_DEBUG_FORWARD_COMPATIBILITY
		attributes |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#endif //! SCION_STRICT_OPENGL_CONTEXT

		SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, attributes );
	}

	// Create the openGL context
	m_pWindow->SetGLContext( SDL_GL_CreateContext( m_pWindow->GetWindow().get() ) );

	// Initialize Glad
	if ( gladLoadGLLoader( SDL_GL_GetProcAddress ) == 0 )
	{
		std::cout << "Failed to Initialize Glad" << std::endl;

		SCION_ERROR( "Failed to LoadGL --> GLAD" );
		return false;
	}

	if ( !m_pWindow->GetGLContext() )
	{
		std::string error = SDL_GetError();
		SCION_ERROR( "Failed to create OpenGL context: {0}", error );
		return false;
	}

	if ( ( SDL_GL_MakeCurrent( m_pWindow->GetWindow().get(), m_pWindow->GetGLContext() ) ) != 0 )
	{
		std::string error = SDL_GetError();
		SCION_ERROR( "Failed to make OpenGL context current: {0}", error );
		return false;
	}

	SDL_GL_SetSwapInterval( 1 );

#ifdef SCION_OPENGL_DEBUG_CALLBACK
	// OpenGL debug callback initialization. A valid current OpenGL context is necessary.
	std::vector<unsigned int> ignore{ /* 1281, 131169, 131185, 131204, 31218*/ };
	SCION_RENDERING::OpenGLDebugger::init( ignore );
	SCION_RENDERING::OpenGLDebugger::breakOnError( false );
	SCION_RENDERING::OpenGLDebugger::breakOnWarning( false );
	SCION_RENDERING::OpenGLDebugger::setSeverityLevel( SCION_RENDERING::OpenGLDebuggerSeverity::Medium );
#endif

	auto& mainRegistry = MAIN_REGISTRY();
	if ( !mainRegistry.Initialize( true ) )
	{
		SCION_ERROR( "Failed to initialize the Main Registry!" );
		return false;
	}

	if ( !Gui::InitImGui( m_pWindow.get() ) )
	{
		SCION_ERROR( "Failed to initialize ImGui!" );
		return false;
	}

	if ( !LoadEditorTextures() )
	{
		SCION_ERROR( "Failed to load the editor textures!" );
		return false;
	}

	mainRegistry.AddToContext<SCION_CORE::ProjectInfoPtr>( std::make_shared<SCION_CORE::ProjectInfo>() );
	m_pHub = std::make_unique<Hub>( *m_pWindow );

	return true;
}

bool Application::InitApp()
{
	if ( !LoadShaders() )
	{
		SCION_ERROR( "Failed to load the shaders!" );
		return false;
	}

	if ( !CreateDisplays() )
	{
		SCION_ERROR( "Failed to create displays." );
		return false;
	}

	if ( !ASSET_MANAGER().CreateDefaultFonts() )
	{
		SCION_ERROR( "Failed to create default fonts" );
		return false;
	}

	auto pEditorFramebuffers = std::make_shared<EditorFramebuffers>();

	if ( !pEditorFramebuffers )
	{
		SCION_ERROR( "Failed to create editor frame buffers!" );
		return false;
	}

	if ( !MAIN_REGISTRY().AddToContext<std::shared_ptr<EditorFramebuffers>>( pEditorFramebuffers ) )
	{
		SCION_ERROR( "Failed add the editor frame buffers to registry context!" );
		return false;
	}

	pEditorFramebuffers->mapFramebuffers.emplace( FramebufferType::SCENE,
												  std::make_shared<SCION_RENDERING::Framebuffer>( 640, 480, false ) );

	pEditorFramebuffers->mapFramebuffers.emplace( FramebufferType::TILEMAP,
												  std::make_shared<SCION_RENDERING::Framebuffer>( 640, 480, false ) );

	if ( !MAIN_REGISTRY().AddToContext<std::shared_ptr<GridSystem>>( std::make_shared<GridSystem>() ) )
	{
		SCION_ERROR( "Failed add the grid system registry context!" );
		return false;
	}

	if ( !MAIN_REGISTRY().AddToContext<EditorRenderSystemPtr>( std::make_shared<EditorRenderSystem>() ) )
	{
		SCION_ERROR( "Failed add the editor render system registry context!" );
		return false;
	}

	auto pPickingTexture = std::make_shared<SCION_RENDERING::PickingTexture>( 640, 480 );
	if ( !pPickingTexture )
	{
		SCION_ERROR( "Failed to create the picking texture." );
		return false;
	}

	if ( !MAIN_REGISTRY().AddToContext<std::shared_ptr<SCION_RENDERING::PickingTexture>>( pPickingTexture ) )
	{
		SCION_ERROR( "Failed to add the picking texture to the registry context!" );
		return false;
	}

	ADD_EVENT_HANDLER( SCION_EDITOR::Events::CloseEditorEvent, &Application::OnCloseEditor, *this );

	// Register Meta Functions
	RegisterEditorMetaFunctions();
	SCION_CORE::CoreEngineData::RegisterMetaFunctions();

	// We can now set the Crash Logger path to the running project
	const auto& sProjectPath = CORE_GLOBALS().GetProjectPath();
	auto& pProjectInfo = MAIN_REGISTRY().GetContext<SCION_CORE::ProjectInfoPtr>();
	SCION_CRASH_LOGGER().SetProjectPath( pProjectInfo->GetProjectPath().string() );

	MAIN_REGISTRY().AddToContext<SharedThreadPool>( std::make_shared<SCION_UTIL::ThreadPool>( 6 ) );

	return true;
}

bool Application::LoadShaders()
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

	if ( !assetManager.AddShaderFromMemory(
			 "picking", SCION_CORE::Shaders::pickingShaderVert, SCION_CORE::Shaders::pickingShaderFrag ) )
	{
		SCION_ERROR( "Failed to add the font shader to the asset manager" );
		return false;
	}

	return true;
}

bool Application::LoadEditorTextures()
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	if ( !assetManager.AddTextureFromMemory(
			 "play_button", EditorTextures::g_PlayButton, EditorTextures::g_PlayButtonSize ) )
	{
		SCION_ERROR( "Failed to load texture [play_button] from memory." );
		return false;
	}

	assetManager.GetTexture( "play_button" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "stop_button", EditorTextures::g_StopButton, EditorTextures::g_StopButtonSize ) )
	{
		SCION_ERROR( "Failed to load texture [stop_button] from memory." );
		return false;
	}

	assetManager.GetTexture( "stop_button" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "music_icon", EditorTextures::g_MusicIcon, EditorTextures::g_MusicIconSize ) )
	{
		SCION_ERROR( "Failed to load texture [music_icon] from memory." );
		return false;
	}

	assetManager.GetTexture( "music_icon" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "scene_icon", EditorTextures::g_SceneIcon, EditorTextures::g_SceneIconSize ) )
	{
		SCION_ERROR( "Failed to load texture [scene_icon] from memory." );
		return false;
	}

	assetManager.GetTexture( "scene_icon" )->SetIsEditorTexture( true );

	// ====== Gizmo Textures Start ======

	if ( !assetManager.AddTextureFromMemory(
			 "S2D_x_axis_translate", EditorTextures::g_XAxisArrow, EditorTextures::g_XAxisArrowSize ) )
	{
		SCION_ERROR( "Failed to load texture [S2D_x_axis_translate] from memory." );
		return false;
	}

	assetManager.GetTexture( "S2D_x_axis_translate" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "S2D_y_axis_translate", EditorTextures::g_YAxisArrow, EditorTextures::g_YAxisArrowSize ) )
	{
		SCION_ERROR( "Failed to load texture [S2D_y_axis_translate] from memory." );
		return false;
	}

	assetManager.GetTexture( "S2D_y_axis_translate" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "S2D_x_axis_scale", EditorTextures::g_XAxisScale, EditorTextures::g_XAxisScaleSize ) )
	{
		SCION_ERROR( "Failed to load texture [S2D_x_axis_scale] from memory." );
		return false;
	}

	assetManager.GetTexture( "S2D_x_axis_scale" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "S2D_y_axis_scale", EditorTextures::g_YAxisScale, EditorTextures::g_YAxisScaleSize ) )
	{
		SCION_ERROR( "Failed to load texture [S2D_y_axis_scale] from memory." );
		return false;
	}

	assetManager.GetTexture( "S2D_y_axis_scale" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "S2D_rotate_tool", EditorTextures::g_RotateTool, EditorTextures::g_RotateToolSize ) )
	{
		SCION_ERROR( "Failed to load texture [S2D_rotate_tool] from memory." );
		return false;
	}

	assetManager.GetTexture( "S2D_rotate_tool" )->SetIsEditorTexture( true );

	// ====== Gizmo Textures End   ======

	// ====== Content Display Textures Start ======
	if ( !assetManager.AddTextureFromMemory(
			 "S2D_file_icon", EditorTextures::g_FileIcon, EditorTextures::g_FileIconSize ) )
	{
		SCION_ERROR( "Failed to load texture [file_icon] from memory." );
		return false;
	}

	assetManager.GetTexture( "S2D_file_icon" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "S2D_music_icon", EditorTextures::g_MusicIcon, EditorTextures::g_MusicIconSize ) )
	{
		SCION_ERROR( "Failed to load texture [music_icon] from memory." );
		return false;
	}

	assetManager.GetTexture( "S2D_music_icon" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "S2D_folder_icon", EditorTextures::g_FolderIcon, EditorTextures::g_FolderIconSize ) )
	{
		SCION_ERROR( "Failed to load texture [folder_icon] from memory." );
		return false;
	}

	assetManager.GetTexture( "S2D_folder_icon" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "S2D_image_icon", EditorTextures::g_ImageIcon, EditorTextures::g_ImageIconSize ) )
	{
		SCION_ERROR( "Failed to load texture [image_icon] from memory." );
		return false;
	}

	assetManager.GetTexture( "S2D_image_icon" )->SetIsEditorTexture( true );
	// ====== Content Display Textures End   ======

	if ( !assetManager.AddTextureFromMemory(
			 "S2D_scion_logo", EditorTextures::g_ScionLogo, EditorTextures::g_ScionLogoSize ) )
	{
		SCION_ERROR( "Failed to load texture [scion_logo] from memory." );
		return false;
	}

	assetManager.GetTexture( "S2D_scion_logo" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "ZZ_S2D_PlayerStart", EditorTextures::g_PlayerStart, EditorTextures::g_PlayerStartSize ) )
	{
		SCION_ERROR( "Failed to load texture [ZZ_S2D_PlayerStart] from memory." );
		return false;
	}

	assetManager.GetTexture( "ZZ_S2D_PlayerStart" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "ZZ_S2D_default_player", EditorTextures::g_DefaultPlayer, EditorTextures::g_DefaultPlayerSize ) )
	{
		SCION_ERROR( "Failed to load texture [ZZ_S2D_default_player] from memory." );
		return false;
	}

	assetManager.GetTexture( "ZZ_S2D_default_player" )->SetIsEditorTexture( true );

	if ( !assetManager.AddCursorFromMemory(
			 "ZZ_S2D_PanningCursor", EditorTextures::g_PanningCursor, EditorTextures::g_PanningCursorSize ) )
	{
		return false;
	}

	return true;
}

void Application::ProcessEvents()
{
	auto& inputManager = SCION_CORE::InputManager::GetInstance();
	auto& keyboard = inputManager.GetKeyboard();
	auto& mouse = inputManager.GetMouse();

	// Process Events
	while ( SDL_PollEvent( &m_Event ) )
	{
		switch ( m_Event.type )
		{
		case SDL_QUIT: m_bIsRunning = false; break;
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
		case SDL_CONTROLLERDEVICEADDED: inputManager.AddGamepad( m_Event.jdevice.which ); break;
		case SDL_CONTROLLERDEVICEREMOVED: inputManager.RemoveGamepad( m_Event.jdevice.which ); break;
		case SDL_JOYAXISMOTION: inputManager.GamepadAxisValues( m_Event ); break;
		case SDL_JOYHATMOTION: inputManager.GamepadHatValues( m_Event ); break;
		case SDL_WINDOWEVENT: {
			switch ( m_Event.window.event )
			{
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				// We only care to control the application window.
				// ImGui handles all separate child windows separately.
				if ( SDL_GetWindowID( m_pWindow->GetWindow().get() ) == m_Event.window.windowID )
				{
					m_pWindow->SetSize( m_Event.window.data1, m_Event.window.data2 );
				}
				break;
			default: break;
			}
			break;
		}
		case SDL_DROPFILE: {
			EVENT_DISPATCHER().EmitEvent( SCION_EDITOR::Events::FileEvent{
				.eAction = Events::EFileAction::FileDropped, .sFilepath = std::string{ m_Event.drop.file } } );

			break;
		}
		default: break;
		}

		// Process ImGui events after other events
		ImGui_ImplSDL2_ProcessEvent( &m_Event );
	}
}

void Application::Update()
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& displayHolder = mainRegistry.GetContext<std::shared_ptr<DisplayHolder>>();

	for ( const auto& pDisplay : displayHolder->displays )
	{
		pDisplay->Update();
	}

	mainRegistry.GetAssetManager().Update();
}

void Application::UpdateInputs()
{
	// Update inputs
	auto& inputManager = SCION_CORE::InputManager::GetInstance();
	auto& keyboard = inputManager.GetKeyboard();
	keyboard.Update();
	auto& mouse = inputManager.GetMouse();
	mouse.Update();

	inputManager.UpdateGamepads();
}

void Application::Render()
{
	Gui::Begin();
	RenderDisplays();
	Gui::End( m_pWindow.get() );

	SDL_GL_SwapWindow( m_pWindow->GetWindow().get() );
}

void Application::CleanUp()
{
	SDL_Quit();
}

bool Application::CreateDisplays()
{
	auto& mainRegistry = MAIN_REGISTRY();

	auto pDisplayHolder = std::make_shared<DisplayHolder>();

	if ( !mainRegistry.AddToContext<std::shared_ptr<DisplayHolder>>( pDisplayHolder ) )
	{
		SCION_ERROR( "Failed to add the display holder to the main registry." );
		return false;
	}

	auto pMenuDisplay = std::make_unique<MenuDisplay>();
	if ( !pMenuDisplay )
	{
		SCION_ERROR( "Failed to Create Menu Display!" );
		return false;
	}

	auto pSceneDisplay = std::make_unique<SceneDisplay>();
	if ( !pSceneDisplay )
	{
		SCION_ERROR( "Failed to Create Scene Display!" );
		return false;
	}

	auto pSceneHierarchyDisplay = std::make_unique<SceneHierarchyDisplay>();
	if ( !pSceneHierarchyDisplay )
	{
		SCION_ERROR( "Failed to Create pSceneHierarchyDisplay !" );
		return false;
	}

	auto pLogDisplay = std::make_unique<LogDisplay>();
	if ( !pLogDisplay )
	{
		SCION_ERROR( "Failed to Create Log Display!" );
		return false;
	}

	auto pTilesetDisplay = std::make_unique<TilesetDisplay>();
	if ( !pTilesetDisplay )
	{
		SCION_ERROR( "Failed to Create TilesetDisplay!" );
		return false;
	}

	auto pTilemapDisplay = std::make_unique<TilemapDisplay>();
	if ( !pTilemapDisplay )
	{
		SCION_ERROR( "Failed to Create TilemapDisplay!" );
		return false;
	}

	auto pTileDetailsDisplay = std::make_unique<TileDetailsDisplay>();
	if ( !pTileDetailsDisplay )
	{
		SCION_ERROR( "Failed to Create TileDetailsDisplay!" );
		return false;
	}

	auto pAssetDisplay = std::make_unique<AssetDisplay>();
	if ( !pAssetDisplay )
	{
		SCION_ERROR( "Failed to Create AssetDisplay!" );
		return false;
	}

	auto pContentDisplay = std::make_unique<ContentDisplay>();
	if ( !pContentDisplay )
	{
		SCION_ERROR( "Failed to Create Content Display!" );
		return false;
	}

	auto pScriptDisplay = std::make_unique<ScriptDisplay>();
	if ( !pScriptDisplay )
	{
		SCION_ERROR( "Failed to Create Script Display!" );
		return false;
	}

	auto pPackageDisplay = std::make_unique<PackageGameDisplay>();
	if ( !pPackageDisplay )
	{
		SCION_ERROR( "Failed to Create Script Display!" );
		return false;
	}

	pDisplayHolder->displays.push_back( std::move( pMenuDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pSceneDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pSceneHierarchyDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pLogDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pTileDetailsDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pTilesetDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pTilemapDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pAssetDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pContentDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pScriptDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pPackageDisplay ) );
	pDisplayHolder->displays.push_back( std::make_unique<ProjectSettingsDisplay>() );

	return true;
}

void Application::InitDisplays()
{
	const auto dockSpaceId = ImGui::DockSpaceOverViewport( 0, ImGui::GetMainViewport() );

	if ( static auto firstTime = true; firstTime ) [[unlikely]]
	{
		firstTime = false;

		ImGui::DockBuilderRemoveNode( dockSpaceId );
		ImGui::DockBuilderAddNode( dockSpaceId );

		auto centerNodeId = dockSpaceId;
		const auto leftNodeId =
			ImGui::DockBuilderSplitNode( centerNodeId, ImGuiDir_Left, 0.2f, nullptr, &centerNodeId );

		auto RightNodeId = ImGui::DockBuilderSplitNode( centerNodeId, ImGuiDir_Right, 0.3f, nullptr, &centerNodeId );

		const auto LogNodeId =
			ImGui::DockBuilderSplitNode( centerNodeId, ImGuiDir_Down, 0.25f, nullptr, &centerNodeId );

		auto TileLayerId = ImGui::DockBuilderSplitNode( RightNodeId, ImGuiDir_Down, 0.4f, nullptr, &RightNodeId );

		ImGui::DockBuilderDockWindow( "Object Details", RightNodeId );
		ImGui::DockBuilderDockWindow( "Tileset", RightNodeId );
		ImGui::DockBuilderDockWindow( "Tile Details", RightNodeId );
		ImGui::DockBuilderDockWindow( ICON_FA_LAYER_GROUP " Tile Layers", TileLayerId );
		ImGui::DockBuilderDockWindow( ICON_FA_SITEMAP " Scene Hierarchy", leftNodeId );
		ImGui::DockBuilderDockWindow( ICON_FA_IMAGE " Scene", centerNodeId );
		ImGui::DockBuilderDockWindow( ICON_FA_CODE " Script List", centerNodeId );
		ImGui::DockBuilderDockWindow( ICON_FA_ARCHIVE " Package Game", centerNodeId );
		ImGui::DockBuilderDockWindow( ICON_FA_MAP " Tilemap Editor", centerNodeId );
		ImGui::DockBuilderDockWindow( ICON_FA_FILE_ALT " Assets", LogNodeId );
		ImGui::DockBuilderDockWindow( ICON_FA_TERMINAL " Logs", LogNodeId );
		ImGui::DockBuilderDockWindow( ICON_FA_FOLDER " Content Browser", LogNodeId );

		ImGui::DockBuilderFinish( dockSpaceId );
	}
}

void Application::RenderDisplays()
{
	InitDisplays();

	auto& mainRegistry = MAIN_REGISTRY();
	auto& pDisplayHolder = mainRegistry.GetContext<std::shared_ptr<DisplayHolder>>();

	for ( const auto& pDisplay : pDisplayHolder->displays )
	{
		pDisplay->Draw();
	}

	// Gui::ShowImGuiDemo();
}

void Application::RegisterEditorMetaFunctions()
{
	DrawComponentsUtil::RegisterUIComponent<SCION_CORE::ECS::Identification>();
	DrawComponentsUtil::RegisterUIComponent<SCION_CORE::ECS::TransformComponent>();
	DrawComponentsUtil::RegisterUIComponent<SCION_CORE::ECS::SpriteComponent>();
	DrawComponentsUtil::RegisterUIComponent<SCION_CORE::ECS::AnimationComponent>();
	DrawComponentsUtil::RegisterUIComponent<SCION_CORE::ECS::PhysicsComponent>();
	DrawComponentsUtil::RegisterUIComponent<SCION_CORE::ECS::TextComponent>();
	DrawComponentsUtil::RegisterUIComponent<SCION_CORE::ECS::RigidBodyComponent>();
	DrawComponentsUtil::RegisterUIComponent<SCION_CORE::ECS::BoxColliderComponent>();
	DrawComponentsUtil::RegisterUIComponent<SCION_CORE::ECS::CircleColliderComponent>();
}

void Application::OnCloseEditor( SCION_EDITOR::Events::CloseEditorEvent& close )
{
	// TODO: Maybe add a check for save??
	m_bIsRunning = false;
}

Application::Application()
	: m_pWindow{ nullptr }
	, m_Event{}
	, m_bIsRunning{ true }
{
}

Application& Application::GetInstance()
{
	static Application app{};
	return app;
}

void Application::Run()
{
	if ( !Initialize() )
	{
		SCION_ERROR( "Initialization Failed!" );
		return;
	}

	if ( !m_pHub || !m_pHub->Run() )
	{
		// If it makes it here, the app is closing.
		return;
	}

	InitApp();

	while ( m_bIsRunning )
	{
		ProcessEvents();
		Update();
		Render();
		UpdateInputs();
		SCENE_MANAGER().UpdateScenes();
	}

	CleanUp();
}
} // namespace SCION_EDITOR
