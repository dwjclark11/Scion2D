#include "Application.h"
#include <glad/glad.h>
#include <Rendering/Utils/OpenGLDebugger.h>
#include <Rendering/Core/Renderer.h>

#include <Logger/Logger.h>
#include <Core/ECS/MainRegistry.h>

#include <Core/Resources/AssetManager.h>
#include <Core/CoreUtilities/CoreEngineData.h>

#include <Core/Systems/ScriptingSystem.h>
#include <Core/Systems/RenderSystem.h>
#include <Core/Systems/RenderUISystem.h>
#include <Core/Systems/RenderShapeSystem.h>
#include <Core/Systems/AnimationSystem.h>
#include <Core/Systems/PhysicsSystem.h>

#include <Core/Scripting/InputManager.h>
#include <Windowing/Inputs/Keyboard.h>
#include <Windowing/Inputs/Mouse.h>
#include <Windowing/Inputs/Gamepad.h>
#include <Windowing/Window/Window.h>

// IMGUI TESTING
// ===================================
#include <imgui_internal.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL_opengl.h>
// ===================================

#include "editor/displays/MenuDisplay.h"
#include "editor/displays/AssetDisplay.h"
#include "editor/displays/SceneDisplay.h"
#include "editor/displays/TilesetDisplay.h"
#include "editor/displays/TilemapDisplay.h"
#include "editor/displays/LogDisplay.h"
#include "editor/utilities/editor_textures.h"
#include "editor/utilities/EditorFramebuffers.h"

#include "editor/systems/GridSystem.h"
#include "editor/scene/SceneManager.h"

namespace SCION_EDITOR
{
bool Application::Initialize()
{
	SCION_INIT_LOGS( false, true );
	// TODO: LOAD CORE ENGINE DATA
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

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode( 0, &displayMode );

	// Create the Window
	m_pWindow = std::make_unique<SCION_WINDOWING::Window>( "SCION 2D",
														   displayMode.w,
														   displayMode.h,
														   SDL_WINDOWPOS_CENTERED,
														   SDL_WINDOWPOS_CENTERED,
														   true,
														   SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
															   SDL_WINDOW_MOUSE_CAPTURE | SDL_WINDOW_MAXIMIZED );

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

	auto renderer = std::make_shared<SCION_RENDERING::Renderer>();

	// Enable Alpha Blending
	renderer->SetCapability( SCION_RENDERING::Renderer::GLCapability::BLEND, true );
	renderer->SetBlendCapability( SCION_RENDERING::Renderer::BlendingFactors::SRC_ALPHA,
								  SCION_RENDERING::Renderer::BlendingFactors::ONE_MINUS_SRC_ALPHA );

	auto& mainRegistry = MAIN_REGISTRY();
	mainRegistry.Initialize();

	if ( !mainRegistry.AddToContext<std::shared_ptr<SCION_RENDERING::Renderer>>( renderer ) )
	{
		SCION_ERROR( "Failed to add the renderer to the registry context!" );
		return false;
	}

	auto renderSystem = std::make_shared<SCION_CORE::Systems::RenderSystem>();
	if ( !renderSystem )
	{
		SCION_ERROR( "Failed to create the render system!" );
		return false;
	}

	if ( !mainRegistry.AddToContext<std::shared_ptr<SCION_CORE::Systems::RenderSystem>>( renderSystem ) )
	{
		SCION_ERROR( "Failed to add the render system to the registry context!" );
		return false;
	}

	auto renderUISystem = std::make_shared<SCION_CORE::Systems::RenderUISystem>();
	if ( !renderUISystem )
	{
		SCION_ERROR( "Failed to create the render UI system!" );
		return false;
	}

	if ( !mainRegistry.AddToContext<std::shared_ptr<SCION_CORE::Systems::RenderUISystem>>( renderUISystem ) )
	{
		SCION_ERROR( "Failed to add the render UI system to the registry context!" );
		return false;
	}

	auto renderShapeSystem = std::make_shared<SCION_CORE::Systems::RenderShapeSystem>();
	if ( !renderShapeSystem )
	{
		SCION_ERROR( "Failed to create the render Shape system!" );
		return false;
	}

	if ( !mainRegistry.AddToContext<std::shared_ptr<SCION_CORE::Systems::RenderShapeSystem>>( renderShapeSystem ) )
	{
		SCION_ERROR( "Failed to add the render Shape system to the registry context!" );
		return false;
	}

	if ( !InitImGui() )
	{
		SCION_ERROR( "Failed to initialize ImGui!" );
		return false;
	}

	if ( !LoadShaders() )
	{
		SCION_ERROR( "Failed to load the shaders!" );
		return false;
	}

	if ( !LoadEditorTextures() )
	{
		SCION_ERROR( "Failed to load the editor textures!" );
		return false;
	}

	if ( !CreateDisplays() )
	{
		SCION_ERROR( "Failed to create displays." );
		return false;
	}

	renderer->SetLineWidth( 4.f );

	if ( !mainRegistry.GetAssetManager().CreateDefaultFonts() )
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

	if ( !mainRegistry.AddToContext<std::shared_ptr<EditorFramebuffers>>( pEditorFramebuffers ) )
	{
		SCION_ERROR( "Failed add the editor frame buffers to registry context!" );
		return false;
	}

	pEditorFramebuffers->mapFramebuffers.emplace( FramebufferType::SCENE,
												  std::make_shared<SCION_RENDERING::Framebuffer>( 640, 480, false ) );

	pEditorFramebuffers->mapFramebuffers.emplace( FramebufferType::TILEMAP,
												  std::make_shared<SCION_RENDERING::Framebuffer>( 640, 480, false ) );

	if ( !mainRegistry.AddToContext<std::shared_ptr<GridSystem>>( std::make_shared<GridSystem>() ) )
	{
		SCION_ERROR( "Failed add the grid system registry context!" );
		return false;
	}

	// TEST SCENES -- TODO: Remove These after testing
	SCENE_MANAGER().AddScene( "DefaultScene" );
	SCENE_MANAGER().AddScene( "NewScene" );

	return true;
}

bool Application::LoadShaders()
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	if ( !assetManager.AddShader( "basic", "assets/shaders/basicShader.vert", "assets/shaders/basicShader.frag" ) )
	{
		SCION_ERROR( "Failed to add the basic shader to the asset manager" );
		return false;
	}

	if ( !assetManager.AddShader( "color", "assets/shaders/colorShader.vert", "assets/shaders/colorShader.frag" ) )
	{
		SCION_ERROR( "Failed to add the color shader to the asset manager" );
		return false;
	}

	if ( !assetManager.AddShader( "circle", "assets/shaders/circleShader.vert", "assets/shaders/circleShader.frag" ) )
	{
		SCION_ERROR( "Failed to add the color shader to the asset manager" );
		return false;
	}

	if ( !assetManager.AddShader( "font", "assets/shaders/fontShader.vert", "assets/shaders/fontShader.frag" ) )
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
			 "play_button", play_button, sizeof( play_button ) / sizeof( play_button[ 0 ] ) ) )
	{
		SCION_ERROR( "Failed to load texture [play_button] from memory." );
		return false;
	}

	assetManager.GetTexture( "play_button" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "stop_button", stop_button, sizeof( stop_button ) / sizeof( stop_button[ 0 ] ) ) )
	{
		SCION_ERROR( "Failed to load texture [stop_button] from memory." );
		return false;
	}

	assetManager.GetTexture( "stop_button" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "music_icon", music_icon, sizeof( music_icon ) / sizeof( music_icon[ 0 ] ) ) )
	{
		SCION_ERROR( "Failed to load texture [music_icon] from memory." );
		return false;
	}

	assetManager.GetTexture( "music_icon" )->SetIsEditorTexture( true );

	if ( !assetManager.AddTextureFromMemory(
			 "scene_icon", scene_icon, sizeof( scene_icon ) / sizeof( scene_icon[ 0 ] ) ) )
	{
		SCION_ERROR( "Failed to load texture [scene_icon] from memory." );
		return false;
	}

	assetManager.GetTexture( "scene_icon" )->SetIsEditorTexture( true );

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
		ImGui_ImplSDL2_ProcessEvent( &m_Event );

		switch ( m_Event.type )
		{
		case SDL_QUIT: m_bIsRunning = false; break;
		case SDL_KEYDOWN: keyboard.OnKeyPressed( m_Event.key.keysym.sym ); break;
		case SDL_KEYUP: keyboard.OnKeyReleased( m_Event.key.keysym.sym ); break;
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
			case SDL_WINDOWEVENT_SIZE_CHANGED: m_pWindow->SetSize( m_Event.window.data1, m_Event.window.data2 ); break;
			default: break;
			}
			break;
		}
		default: break;
		}
	}
}

void Application::Update()
{
	auto& engineData = SCION_CORE::CoreEngineData::GetInstance();
	engineData.UpdateDeltaTime();

	auto& mainRegistry = MAIN_REGISTRY();
	auto& displayHolder = mainRegistry.GetContext<std::shared_ptr<DisplayHolder>>();

	for ( const auto& pDisplay : displayHolder->displays )
		pDisplay->Update();

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
	Begin();
	RenderImGui();
	End();

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

	auto pAssetDisplay = std::make_unique<AssetDisplay>();
	if ( !pAssetDisplay )
	{
		SCION_ERROR( "Failed to Create AssetDisplay!" );
		return false;
	}

	pDisplayHolder->displays.push_back( std::move( pMenuDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pSceneDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pLogDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pTilesetDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pTilemapDisplay ) );
	pDisplayHolder->displays.push_back( std::move( pAssetDisplay ) );

	return true;
}

bool Application::InitImGui()
{
	const char* glslVersion = "#version 450";
	IMGUI_CHECKVERSION();

	if ( !ImGui::CreateContext() )
	{
		SCION_ERROR( "Failed to create ImGui Context" );
		return false;
	}

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.ConfigWindowsMoveFromTitleBarOnly = true;

	if ( !ImGui_ImplSDL2_InitForOpenGL( m_pWindow->GetWindow().get(), m_pWindow->GetGLContext() ) )
	{
		SCION_ERROR( "Failed to intialize ImGui SDL2 for OpenGL!" );
		return false;
	}

	if ( !ImGui_ImplOpenGL3_Init( glslVersion ) )
	{
		SCION_ERROR( "Failed to intialize ImGui OpenGL3!" );
		return false;
	}

	return true;
}

void Application::Begin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void Application::End()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

	ImGuiIO& io = ImGui::GetIO();
	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		SDL_GLContext backupContext = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		SDL_GL_MakeCurrent( m_pWindow->GetWindow().get(), backupContext );
	}
}

void Application::RenderImGui()
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

		const auto RightNodeId =
			ImGui::DockBuilderSplitNode( centerNodeId, ImGuiDir_Right, 0.2f, nullptr, &centerNodeId );

		const auto LogNodeId =
			ImGui::DockBuilderSplitNode( centerNodeId, ImGuiDir_Down, 0.25f, nullptr, &centerNodeId );
		ImGui::DockBuilderDockWindow( "Tileset", RightNodeId );
		ImGui::DockBuilderDockWindow( "Dear ImGui Demo", leftNodeId );
		ImGui::DockBuilderDockWindow( "Scene", centerNodeId );
		ImGui::DockBuilderDockWindow( "Tilemap Editor", centerNodeId );
		ImGui::DockBuilderDockWindow( "Assets", LogNodeId );
		ImGui::DockBuilderDockWindow( "Logs", LogNodeId );

		ImGui::DockBuilderFinish( dockSpaceId );
	}

	auto& mainRegistry = MAIN_REGISTRY();
	auto& pDisplayHolder = mainRegistry.GetContext<std::shared_ptr<DisplayHolder>>();

	for ( const auto& pDisplay : pDisplayHolder->displays )
	{
		pDisplay->Draw();
	}

	ImGui::ShowDemoWindow();
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

	while ( m_bIsRunning )
	{
		ProcessEvents();
		Update();
		Render();
	}

	CleanUp();
}
} // namespace SCION_EDITOR
