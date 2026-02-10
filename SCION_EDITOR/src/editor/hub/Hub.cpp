#include "editor/hub/Hub.h"
#include <Windowing/Window/Window.h>
#include "editor/utilities/imgui/Gui.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/loaders/ProjectLoader.h"
#include "ScionFilesystem/Dialogs/FileDialog.h"
#include "Logger/Logger.h"

#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"

#include "ScionUtilities/HelperUtilities.h"

#include <Rendering/Essentials/Texture.h>

// IMGUI
// ===================================
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL_opengl.h>
// ===================================

constexpr ImVec2 BUTTON_SIZE = ImVec2{ 100.f, 20.f };

namespace Scion::Editor
{

Hub::Hub( Scion::Windowing::Window& window )
	: m_Window{ window }
	, m_bRunning{ false }
	, m_bLoadError{ false }
	, m_Event{}
	, m_eState{ EHubState::Default }
	, m_Width{ static_cast<float>( m_Window.GetWidth() ) }
	, m_Height{ static_cast<float>( m_Window.GetHeight() ) }
	, m_sNewProjectName{}
	, m_sNewProjectPath{ DEFAULT_PROJECT_PATH }
	, m_sPrevProjectPath{}
	, m_sPrevProjectName{}
{
	fs::path projectPath{ m_sNewProjectPath };
	if ( !fs::exists( projectPath ) )
	{
		std::error_code ec;
		if ( !fs::create_directories( projectPath, ec ) )
		{
			SCION_ERROR( "HUB - Failed to create directories. Error: {}", ec.message() );
			m_eState = EHubState::Close;
			// TODO: Close and actually log an error.
		}
	}
}

Hub::~Hub()
{
}

bool Hub::Run()
{
	m_bRunning = Initialize();

	while ( m_bRunning )
	{
		ProcessEvents();
		Update();
		Render();
	}

	bool bClosed{ m_eState == EHubState::Close };

	if ( !bClosed )
	{
		SDL_SetWindowBordered( m_Window.GetWindow().get(), SDL_TRUE );
		SDL_SetWindowResizable( m_Window.GetWindow().get(), SDL_TRUE );
		SDL_MaximizeWindow( m_Window.GetWindow().get() );

		std::string sTitle{ "Scion2D - " };
		sTitle += !m_sNewProjectName.empty() ? m_sNewProjectName : m_sPrevProjectName;
		SDL_SetWindowTitle( m_Window.GetWindow().get(), sTitle.c_str() );
	}

	return !bClosed;
}

bool Hub::Initialize()
{
	return true;
}

void Hub::DrawGui()
{
	if ( !ImGui::Begin( "##_HudDisplay",
						nullptr,
						ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration ) )
	{
		ImGui::End();
		return;
	}

	ImGui::SetWindowPos( ImGui::GetMainViewport()->Pos );
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImGui::SetWindowSize( ImVec2{ m_Width, m_Height } );

	// Draw the Scion2D LOGO
	auto pLogo = ASSET_MANAGER().GetTexture( "S2D_scion_logo" );
	if ( pLogo )
	{
		ImGui::SetCursorPos( ImVec2{ m_Width * 0.5f - pLogo->GetWidth(), 50.f } );
		ImGui::Image(
			(ImTextureID)(intptr_t)pLogo->GetID(),
			ImVec2{ static_cast<float>( pLogo->GetWidth() * 2.f ), static_cast<float>( pLogo->GetHeight() * 2.f ) } );
	}

	switch ( m_eState )
	{
	case EHubState::Default: DrawDefault(); break;
	case EHubState::NewProject: DrawNewProject(); break;
	case EHubState::OpenProject: DrawOpenProject(); break;
	default: break;
	}

	ImGui::End();
}

void Hub::DrawDefault()
{
	ImGui::SetCursorPos( ImVec2{ m_Width * 0.5f - BUTTON_SIZE.x, 300.f } );
	if ( ImGui::Button( "New Project", BUTTON_SIZE ) )
	{
		m_eState = EHubState::NewProject;
	}

	ImGui::SameLine();
	if ( ImGui::Button( "Open Project", BUTTON_SIZE ) )
	{
		m_eState = EHubState::OpenProject;
	}

	ImGui::SetCursorPos( ImVec2{ m_Width - ( BUTTON_SIZE.x + 16.f ), m_Height - ( BUTTON_SIZE.y + 16.f ) } );
	if ( ImGui::Button( "Close", BUTTON_SIZE ) )
	{
		m_eState = EHubState::Close;
		m_bRunning = false;
	}
}

void Hub::DrawNewProject()
{
	ImGui::SetCursorPosY( 300.f );
	ImGui::OffsetTextX( "New Project", 32.f );
	ImGui::SameLine();

	char newProjectBuffer[ 256 ]{ 0 };
	char newProjectPathBuffer[ 256 ]{ 0 };
#ifdef _WIN32
	strcpy_s( newProjectBuffer, m_sNewProjectName.c_str() );
	strcpy_s( newProjectPathBuffer, m_sNewProjectPath.c_str() );
#else
	strcpy( newProjectBuffer, m_sNewProjectName.c_str() );
	strcpy( newProjectPathBuffer, m_sNewProjectPath.c_str() );
#endif
	ImGui::SetCursorPosX( 128.f );
	if ( ImGui::InputText( "##ProjectName", newProjectBuffer, sizeof( newProjectBuffer ) ) )
	{
		m_sNewProjectName = std::string{ newProjectBuffer };
	}

	ImGui::OffsetTextX( "Location", 32.f );
	ImGui::SameLine();
	ImGui::SetCursorPosX( 128.f );
	if ( ImGui::InputText( "##ProjectPath", newProjectPathBuffer, sizeof( newProjectPathBuffer ) ) )
	{
		m_sNewProjectPath = std::string{ newProjectPathBuffer };
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Browse" ) )
	{
		Scion::Filesystem::FileDialog fd{};
		const auto sFilepath = fd.SelectFolderDialog( "Open", BASE_PATH );
		if ( !sFilepath.empty() )
		{
#ifdef _WIN32
			strcpy_s( newProjectPathBuffer, sFilepath.c_str() );
#else
			strcpy( newProjectPathBuffer, sFilepath.c_str() );
#endif
			m_sNewProjectPath = sFilepath;
		}
	}

	if ( !m_sNewProjectName.empty() && !m_sNewProjectPath.empty() )
	{
		fs::path projectPath{ m_sNewProjectPath };

		if ( fs::exists( projectPath ) )
		{
			if ( IsReservedPathOrFile( fs::path{ m_sNewProjectPath } ) )
			{
				ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f },
									"Path [%s] is a reserved path. Please change paths.",
									m_sNewProjectPath.c_str() );
			}
			else
			{
				ImGui::SetCursorPosX( 128.f );
				if ( ImGui::Button( "Create", BUTTON_SIZE ) )
				{
					// Create the project
					ProjectLoader pl{};
					if ( !pl.CreateNewProject( m_sNewProjectName, m_sNewProjectPath ) )
					{
						// TODO: show an error
					}
					else
					{

						m_bRunning = false;
						m_eState = EHubState::CreateNew;
					}
				}
			}
		}
	}

	ImGui::SetCursorPos( ImVec2{ m_Width - ( BUTTON_SIZE.x + 16.f ), m_Height - ( BUTTON_SIZE.y + 16.f ) } );
	if ( ImGui::Button( "Cancel", BUTTON_SIZE ) )
	{
		m_eState = EHubState::Default;
		m_sNewProjectName.clear();
		m_sNewProjectPath = DEFAULT_PROJECT_PATH;
	}
}

void Hub::DrawOpenProject()
{
	ImGui::SetCursorPos( ImVec2{ 200.f, 300.f } );
	ImGui::Text( "Project" );
	ImGui::SameLine();
	char prevProjectBuffer[ 256 ]{ 0 };
#ifdef _WIN32
	strcpy_s( prevProjectBuffer, m_sPrevProjectName.c_str() );
#else
	strcpy( prevProjectBuffer, m_sPrevProjectName.c_str() );
#endif
	ImGui::PushItemWidth( 200.f );
	ImGui::SetCursorPosX( 300.f );
	if ( ImGui::InputText( "##PrevProjectName", prevProjectBuffer, sizeof( prevProjectBuffer ) ) )
	{
		m_sPrevProjectName = std::string{ prevProjectBuffer };
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Browse" ) )
	{
		Scion::Filesystem::FileDialog fd{};
		const auto sFilepath = fd.OpenFileDialog( "Open", "", { "*.s2dprj" } );
		if ( !sFilepath.empty() )
		{
#ifdef _WIN32
			strcpy_s( prevProjectBuffer, sFilepath.c_str() );
#else
			strcpy( prevProjectBuffer, sFilepath.c_str() );
#endif
			m_sPrevProjectPath = sFilepath;

			fs::path path{ sFilepath };
			m_sPrevProjectName = path.stem().string();
		}
	}

	if ( !m_sPrevProjectPath.empty() && fs::exists( fs::path{ m_sPrevProjectPath } ) )
	{
		ImGui::SetCursorPos( ImVec2{ 300.f, 432.f } );
		if ( ImGui::Button( "Load Project", BUTTON_SIZE ) )
		{
			// Create the project
			ProjectLoader pl{};
			if ( !pl.LoadProject( m_sPrevProjectPath ) )
			{
				// TODO: show an error
			}
			else
			{

				m_bRunning = false;
				m_eState = EHubState::OpenProject;
			}
		}
	}

	ImGui::SetCursorPos( ImVec2{ m_Width - ( BUTTON_SIZE.x + 16.f ), m_Height - ( BUTTON_SIZE.y + 16.f ) } );
	if ( ImGui::Button( "Cancel", BUTTON_SIZE ) )
	{
		m_eState = EHubState::Default;
		m_sPrevProjectName.clear();
		m_sPrevProjectPath.clear();
	}

	ImGui::PopItemWidth();
}

void Hub::ProcessEvents()
{
	// Process Events
	while ( SDL_PollEvent( &m_Event ) )
	{
		ImGui_ImplSDL2_ProcessEvent( &m_Event );

		switch ( m_Event.type )
		{
		case SDL_QUIT: m_bRunning = false; break;
		case SDL_KEYDOWN:		  // keyboard.OnKeyPressed( m_Event.key.keysym.sym ); break;
		case SDL_KEYUP:			  // keyboard.OnKeyReleased( m_Event.key.keysym.sym ); break;
		case SDL_MOUSEBUTTONDOWN: // mouse.OnBtnPressed( m_Event.button.button ); break;
		case SDL_MOUSEBUTTONUP:	  // mouse.OnBtnReleased( m_Event.button.button ); break;
		default: break;
		}
	}
}

void Hub::Update()
{
}

void Hub::Render()
{
	Gui::Begin();
	DrawGui();
	Gui::End( &m_Window );

	SDL_GL_SwapWindow( m_Window.GetWindow().get() );
}

} // namespace Scion::Editor
