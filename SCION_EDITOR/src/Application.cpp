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
#include <Core/Systems/RenderPickingSystem.h>

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
#include "editor/displays/ProfilerDisplay.h"

#include "editor/scene/SceneManager.h"

#include "editor/utilities/editor_textures.h"
#include "editor/utilities/EditorState.h"
#include "editor/utilities/EditorFramebuffers.h"
#include "editor/utilities/DrawComponentUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"

#include "Core/CoreUtilities/ProjectInfo.h"
#include "editor/systems/GridSystem.h"
#include "editor/systems/EditorRenderSystem.h"

#include "editor/events/EditorEventTypes.h"
#include "Core/Events/EventDispatcher.h"
#include "Core/Events/EngineEventTypes.h"

#include "Core/Profiling/ProfileCollector.h"

#include "ScionUtilities/ThreadPool.h"
#include "ScionUtilities/HelperUtilities.h"
#include "editor/hub/Hub.h"

// IMGUI
// ===================================
#include <imgui_internal.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include "editor/utilities/imgui/Gui.h"
// ===================================

static constexpr float FOOTER_HEIGHT = 24.f;

namespace Scion::Editor
{
bool Application::Initialize()
{
	bool bDebug{ true };

#ifdef NDEBUG
	bDebug = false;
#endif

	SCION_INIT_LOGS( bDebug, true );
	SCION_INIT_CRASH_LOGS();

	// Init SDL
	if ( !SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK ) )
	{
		std::string error = SDL_GetError();
		SCION_ERROR( "Failed to initialize SDL: {0}", error );
		return false;
	}

	// Set up OpenGL
	if ( !SDL_GL_LoadLibrary( NULL ) )
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
	m_pWindow = std::make_unique<Scion::Windowing::Window>( "SCION 2D", 1366, 768, true, SDL_WINDOW_OPENGL );

	/*
	 * SDL Hack - If we create the window as borderless, we lose the icon in the title bar.
	 * Therefore, after testing I found that if we create the window with a border, then hide
	 * the border. When we show the border later on, the icon will be there.
	 */
	SDL_SetWindowBordered( m_pWindow->GetWindow().get(), false );

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
	if ( !gladLoadGLLoader( (GLADloadproc)SDL_GL_GetProcAddress ) )
	{
		SCION_ERROR( "Failed to initialize GLAD" );
		return 1;
	}

	if ( !m_pWindow->GetGLContext() )
	{
		std::string error = SDL_GetError();
		SCION_ERROR( "Failed to create OpenGL context: {0}", error );
		return false;
	}

	if ( !SDL_GL_MakeCurrent( m_pWindow->GetWindow().get(), m_pWindow->GetGLContext() ) )
	{
		std::string error = SDL_GetError();
		SCION_ERROR( "Failed to make OpenGL context current: {0}", error );
		return false;
	}

	SDL_GL_SetSwapInterval( 1 );

#ifdef SCION_OPENGL_DEBUG_CALLBACK
	// OpenGL debug callback initialization. A valid current OpenGL context is necessary.
	std::vector<unsigned int> ignore{ /* 1281, 131169, 131185, 131204, 31218*/ };
	Scion::Rendering::OpenGLDebugger::init( ignore );
	Scion::Rendering::OpenGLDebugger::breakOnError( false );
	Scion::Rendering::OpenGLDebugger::breakOnWarning( false );
	Scion::Rendering::OpenGLDebugger::setSeverityLevel( Scion::Rendering::OpenGLDebuggerSeverity::Medium );
#endif

	// Initialize the mixer. Right now we only use SDL3_mixer, I may adjust to use other types in the future
	if ( !MIX_Init() )
	{
		SCION_ERROR( "MIX_Init failed: {}", SDL_GetError() );
	}

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

	mainRegistry.AddToContext( std::make_unique<Scion::Core::Systems::RenderPickingSystem>() );
	mainRegistry.AddToContext( std::make_unique<Scion::Core::ProjectInfo>() );
	mainRegistry.AddToContext( std::make_unique<EditorState>() );

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

	auto& mainRegistry = MAIN_REGISTRY();

	auto& pEditorFramebuffers = mainRegistry.AddToContext( std::make_unique<EditorFramebuffers>() );

	pEditorFramebuffers->mapFramebuffers.emplace( FramebufferType::SCENE,
												  std::make_shared<Scion::Rendering::Framebuffer>( 640, 480, false ) );

	pEditorFramebuffers->mapFramebuffers.emplace( FramebufferType::TILEMAP,
												  std::make_shared<Scion::Rendering::Framebuffer>( 640, 480, false ) );

	mainRegistry.AddToContext( std::make_unique<GridSystem>() );
	mainRegistry.AddToContext( std::make_unique<EditorRenderSystem>() );
	mainRegistry.AddToContext( std::make_unique<Scion::Rendering::PickingTexture>( 640, 480 ) );
	

	ADD_EVENT_HANDLER( Scion::Editor::Events::CloseEditorEvent, &Application::OnCloseEditor, *this );

	// Register Meta Functions
	RegisterEditorMetaFunctions();
	Scion::Core::CoreEngineData::RegisterMetaFunctions();

	// We can now set the Crash Logger path to the running project
	const auto& sProjectPath = CORE_GLOBALS().GetProjectPath();
	auto& pProjectInfo = mainRegistry.GetContext<Scion::Core::ProjectInfoPtr>();
	SCION_CRASH_LOGGER().SetProjectPath( pProjectInfo->GetProjectPath().string() );

	mainRegistry.AddToContext( std::make_unique<Scion::Utilities::ThreadPool>( 6 ) );

	return true;
}

bool Application::LoadShaders()
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	if ( !assetManager.AddShaderFromMemory(
			 "basic", Scion::Core::Shaders::basicShaderVert, Scion::Core::Shaders::basicShaderFrag ) )
	{
		SCION_ERROR( "Failed to add the basic shader to the asset manager" );
		return false;
	}

	if ( !assetManager.AddShaderFromMemory(
			 "color", Scion::Core::Shaders::colorShaderVert, Scion::Core::Shaders::colorShaderFrag ) )
	{
		SCION_ERROR( "Failed to add the color shader to the asset manager" );
		return false;
	}

	if ( !assetManager.AddShaderFromMemory(
			 "circle", Scion::Core::Shaders::circleShaderVert, Scion::Core::Shaders::circleShaderFrag ) )
	{
		SCION_ERROR( "Failed to add the color shader to the asset manager" );
		return false;
	}

	if ( !assetManager.AddShaderFromMemory(
			 "font", Scion::Core::Shaders::fontShaderVert, Scion::Core::Shaders::fontShaderFrag ) )
	{
		SCION_ERROR( "Failed to add the font shader to the asset manager" );
		return false;
	}

	if ( !assetManager.AddShaderFromMemory(
			 "picking", Scion::Core::Shaders::pickingShaderVert, Scion::Core::Shaders::pickingShaderFrag ) )
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
	auto& inputManager = Scion::Core::InputManager::GetInstance();
	auto& keyboard = inputManager.GetKeyboard();
	auto& mouse = inputManager.GetMouse();

	// Process Events
	while ( SDL_PollEvent( &m_Event ) )
	{
		switch ( m_Event.type )
		{
		case SDL_EVENT_QUIT: m_bIsRunning = false; break;
		case SDL_EVENT_KEY_DOWN:
			keyboard.OnKeyPressed( m_Event.key.key );
			EVENT_DISPATCHER().EmitEvent( Scion::Core::Events::KeyEvent{
				.key = static_cast<int>( m_Event.key.key ), .eType = Scion::Core::Events::EKeyEventType::Pressed } );
			break;
		case SDL_EVENT_KEY_UP:
			keyboard.OnKeyReleased( m_Event.key.key );
			EVENT_DISPATCHER().EmitEvent( Scion::Core::Events::KeyEvent{
				.key = static_cast<int>( m_Event.key.key ), .eType = Scion::Core::Events::EKeyEventType::Released } );
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN: mouse.OnBtnPressed( m_Event.button.button ); break;
		case SDL_EVENT_MOUSE_BUTTON_UP: mouse.OnBtnReleased( m_Event.button.button ); break;
		case SDL_EVENT_MOUSE_WHEEL:
			mouse.SetMouseWheelX( m_Event.wheel.x );
			mouse.SetMouseWheelY( m_Event.wheel.y );
			break;
		case SDL_EVENT_MOUSE_MOTION: mouse.SetMouseMoving( true ); break;
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN: inputManager.GamepadBtnPressed( m_Event ); break;
		case SDL_EVENT_GAMEPAD_BUTTON_UP: inputManager.GamepadBtnReleased( m_Event ); break;
		case SDL_EVENT_GAMEPAD_ADDED: {
			int index = inputManager.AddGamepad( m_Event.jdevice.which );
			if ( index > 0 )
			{
				EVENT_DISPATCHER().EmitEvent( Scion::Core::Events::GamepadConnectEvent{
					.eConnectType = Scion::Core::Events::EGamepadConnectType::Connected, .index = index } );
			}

			break;
		}
		case SDL_EVENT_GAMEPAD_REMOVED: {
			int index = inputManager.RemoveGamepad( m_Event.jdevice.which );

			if ( index > 0 )
			{
				EVENT_DISPATCHER().EmitEvent( Scion::Core::Events::GamepadConnectEvent{
					.eConnectType = Scion::Core::Events::EGamepadConnectType::Disconnected, .index = index } );
			}

			break;
		}
		case SDL_EVENT_JOYSTICK_AXIS_MOTION: inputManager.GamepadAxisValues( m_Event ); break;
		case SDL_EVENT_JOYSTICK_HAT_MOTION: inputManager.GamepadHatValues( m_Event ); break;
		case SDL_EVENT_WINDOW_RESIZED: {
			// We only care to control the application window.
			// ImGui handles all separate child windows separately.
			if ( SDL_GetWindowID( m_pWindow->GetWindow().get() ) == m_Event.window.windowID )
			{
				m_pWindow->SetSize( m_Event.window.data1, m_Event.window.data2 );
			}
			break;
		}
		case SDL_EVENT_DROP_FILE: {
			EVENT_DISPATCHER().EmitEvent( Scion::Editor::Events::FileEvent{
				.eAction = Events::EFileAction::FileDropped, .sFilepath = std::string{ m_Event.drop.data } } );

			break;
		}
		default: break;
		}

		// Process ImGui events after other events
		ImGui_ImplSDL3_ProcessEvent( &m_Event );
	}
}

void Application::Update()
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& displayHolder = mainRegistry.GetContext<std::unique_ptr<DisplayHolder>>();

	for ( const auto& pDisplay : displayHolder->displays )
	{
		pDisplay->Update();
	}

	mainRegistry.GetAssetManager().Update();
}

void Application::UpdateInputs()
{
	// Update inputs
	auto& inputManager = Scion::Core::InputManager::GetInstance();
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
	// Save the editor state.
	auto& pProjectInfo = MAIN_REGISTRY().GetContext<Scion::Core::ProjectInfoPtr>();
	auto& pEditorState = MAIN_REGISTRY().GetContext<EditorStatePtr>();
	pEditorState->Save( *pProjectInfo );

	// Cleanup the registry before we shutdown sdl
	MAIN_REGISTRY().CleanUp();

	SDL_GL_DestroyContext( m_pWindow->GetGLContext() );
	SDL_DestroyWindow( m_pWindow->GetWindow().get() );

	SDL_Quit();
}

bool Application::CreateDisplays()
{
	auto& mainRegistry = MAIN_REGISTRY();

	auto& pDisplayHolder = mainRegistry.AddToContext( std::make_unique<DisplayHolder>() );
	
	pDisplayHolder->displays.push_back( std::make_unique<MenuDisplay>() );
	pDisplayHolder->displays.push_back( std::make_unique<SceneDisplay>() );
	pDisplayHolder->displays.push_back( std::make_unique<SceneHierarchyDisplay>() );
	pDisplayHolder->displays.push_back( std::make_unique<LogDisplay>() );
	pDisplayHolder->displays.push_back( std::make_unique<TilesetDisplay>() );
	pDisplayHolder->displays.push_back( std::make_unique<TilemapDisplay>() );
	pDisplayHolder->displays.push_back( std::make_unique<TileDetailsDisplay>() );
	pDisplayHolder->displays.push_back( std::make_unique<AssetDisplay>() );
	pDisplayHolder->displays.push_back( std::make_unique<ContentDisplay>() );
	pDisplayHolder->displays.push_back( std::make_unique<ScriptDisplay>() );
	pDisplayHolder->displays.push_back( std::make_unique<PackageGameDisplay>() );

	pDisplayHolder->displays.push_back( std::make_unique<ProjectSettingsDisplay>() );
	pDisplayHolder->displays.push_back( std::make_unique<ProfilerDisplay>() );

	return true;
}

void Application::InitDisplays()
{
	ImGuiViewport* pViewport = ImGui::GetMainViewport();
	pViewport->WorkSize.y -= FOOTER_HEIGHT;
	const auto dockSpaceId = ImGui::DockSpaceOverViewport( 0, ImGui::GetMainViewport() );

	static auto firstTime = true;

	if ( firstTime )
	{
		firstTime = false;

		ImGui::DockBuilderRemoveNode( dockSpaceId );
		ImGui::DockBuilderAddNode( dockSpaceId );

		auto topNodeId = dockSpaceId;

		// -------------------------------------------------------------
		// Top main tabs: Project Settings, Package Game, ScriptList
		// PLUS a tab called Scene Editor (this will host the inner dock)
		// -------------------------------------------------------------
		ImGui::DockBuilderDockWindow( "###ProjectSettingsDisplay", topNodeId );
		ImGui::DockBuilderDockWindow( ICON_FA_CODE " Script List", topNodeId );
		ImGui::DockBuilderDockWindow( ICON_FA_ARCHIVE " Package Game", topNodeId );
		ImGui::DockBuilderDockWindow( ICON_FA_VECTOR_SQUARE " Scene Editor", topNodeId );

		ImGui::DockBuilderFinish( dockSpaceId );
	}

	// ---- Scene Editor inner dockspace ----
	ImGui::Begin( ICON_FA_VECTOR_SQUARE " Scene Editor" );

	static ImGuiID innerDockId = 0;

	if ( innerDockId == 0 )
	{
		// Create an inner dockspace
		innerDockId = ImGui::GetID( "SceneEditorDock" );

		ImGui::DockBuilderRemoveNode( innerDockId );
		ImGui::DockBuilderAddNode( innerDockId );

		auto centerId = innerDockId;

		auto bottomNodeId = ImGui::DockBuilderSplitNode( centerId, ImGuiDir_Down, 0.25f, nullptr, &centerId );
		auto leftId = ImGui::DockBuilderSplitNode( centerId, ImGuiDir_Left, 0.22f, nullptr, &centerId );
		auto rightId = ImGui::DockBuilderSplitNode( centerId, ImGuiDir_Right, 0.28f, nullptr, &centerId );
		auto rightBottomId = ImGui::DockBuilderSplitNode( rightId, ImGuiDir_Down, 0.40f, nullptr, &rightId );

		// -------------------------------------------------------------
		// Bottom tabs: Logs / Assets / Content Browser
		// -------------------------------------------------------------
		ImGui::DockBuilderDockWindow( ICON_FA_TERMINAL " Logs", bottomNodeId );
		ImGui::DockBuilderDockWindow( "###AssetDisplay", bottomNodeId );
		ImGui::DockBuilderDockWindow( "###ContentDisplay", bottomNodeId );

		// LEFT
		ImGui::DockBuilderDockWindow( "###SceneHierarchyDisplay", leftId );

		// CENTER
		ImGui::DockBuilderDockWindow( ICON_FA_IMAGE " Scene", centerId );
		ImGui::DockBuilderDockWindow( "###TilemapDisplay", centerId );

		// RIGHT (top)
		ImGui::DockBuilderDockWindow( ICON_FA_TH " Tileset", rightId );
		ImGui::DockBuilderDockWindow( ICON_FA_CLIPBOARD_LIST " Tile Details", rightId );
		ImGui::DockBuilderDockWindow( ICON_FA_LIST " Object Details", rightId );

		// RIGHT (bottom)
		ImGui::DockBuilderDockWindow( ICON_FA_LAYER_GROUP " Tile Layers", rightBottomId );

		ImGui::DockBuilderFinish( innerDockId );
	}

	ImGui::DockSpace( innerDockId );

	ImGui::End();
}

void Application::RenderDisplays()
{
	InitDisplays();

	auto& mainRegistry = MAIN_REGISTRY();
	auto& pDisplayHolder = mainRegistry.GetContext<std::unique_ptr<DisplayHolder>>();

	for ( const auto& pDisplay : pDisplayHolder->displays )
	{
		pDisplay->Draw();
	}

	// Gui::ShowImGuiDemo();
}

void Application::RegisterEditorMetaFunctions()
{
	DrawComponentsUtil::RegisterUIComponent<Scion::Core::ECS::Identification>();
	DrawComponentsUtil::RegisterUIComponent<Scion::Core::ECS::TransformComponent>();
	DrawComponentsUtil::RegisterUIComponent<Scion::Core::ECS::SpriteComponent>();
	DrawComponentsUtil::RegisterUIComponent<Scion::Core::ECS::AnimationComponent>();
	DrawComponentsUtil::RegisterUIComponent<Scion::Core::ECS::PhysicsComponent>();
	DrawComponentsUtil::RegisterUIComponent<Scion::Core::ECS::TextComponent>();
	DrawComponentsUtil::RegisterUIComponent<Scion::Core::ECS::RigidBodyComponent>();
	DrawComponentsUtil::RegisterUIComponent<Scion::Core::ECS::BoxColliderComponent>();
	DrawComponentsUtil::RegisterUIComponent<Scion::Core::ECS::CircleColliderComponent>();
}

void Application::OnCloseEditor( Scion::Editor::Events::CloseEditorEvent& close )
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
		PROFILE_COLLECTOR().BeginFrame();

		ProcessEvents();
		Update();
		Render();
		UpdateInputs();
		SCENE_MANAGER().UpdateScenes();
		PROFILE_COLLECTOR().EndFrame();
		SCION_PROFILE_FRAME();
	}

	CleanUp();
}
} // namespace Scion::Editor
