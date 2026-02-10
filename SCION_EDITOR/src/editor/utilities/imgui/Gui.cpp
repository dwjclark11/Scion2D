#include "editor/utilities/imgui/Gui.h"
#include "Logger/Logger.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/utilities/fonts/editor_fonts.h"
#include "Core/Resources/fonts/default_fonts.h"
#include <Windowing/Window/Window.h>

// IMGUI
// ===================================
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "imgui.h"
#include <imgui_internal.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL_opengl.h>
// ===================================

namespace Scion::Editor
{
bool Gui::InitImGui( Scion::Windowing::Window* pWindow )
{
	if ( m_bInitialized )
		return false;

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

	ImFont* pDefaultFont = io.Fonts->AddFontDefault();
	ImGui::AddFont( "default", pDefaultFont, 13.f );

	float baseFontSize = 16.0f;
	float iconFontSize = baseFontSize * 2.0f / 3.0f;

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = iconFontSize;
	icons_config.GlyphOffset = ImVec2{ 0.f, 2.f };
	io.Fonts->AddFontFromMemoryTTF(
		EditorFonts::g_FaSolid900, EditorFonts::g_FaSolid900Size, baseFontSize, &icons_config, icons_ranges );

	ImGui::AddFontFromMemory( "roboto-bold-14",
							  SCION_RESOURCES::CoreFonts::g_RobotoBoldFont,
							  SCION_RESOURCES::CoreFonts::g_RobotoBoldFontSize,
							  14.f );

	ImGui::AddFontFromMemory( "roboto-bold-24",
							  SCION_RESOURCES::CoreFonts::g_RobotoBoldFont,
							  SCION_RESOURCES::CoreFonts::g_RobotoBoldFontSize,
							  24.f );

	if ( !ImGui_ImplSDL2_InitForOpenGL( pWindow->GetWindow().get(), pWindow->GetGLContext() ) )
	{
		SCION_ERROR( "Failed to intialize ImGui SDL2 for OpenGL!" );
		return false;
	}

	if ( !ImGui_ImplOpenGL3_Init( glslVersion ) )
	{
		SCION_ERROR( "Failed to intialize ImGui OpenGL3!" );
		return false;
	}

	ImGui::InitDefaultStyles();
	return true;
}
void Gui::Begin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void Gui::End( Scion::Windowing::Window* pWindow )
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

	ImGuiIO& io = ImGui::GetIO();
	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		SDL_Window* backupCurrentWindow = SDL_GL_GetCurrentWindow();
		SDL_GLContext backupCurrentContext = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent( backupCurrentWindow, backupCurrentContext );
	}
}

void Gui::ShowImGuiDemo()
{
	ImGui::ShowDemoWindow();
}

} // namespace Scion::Editor
