#include "Gui.h"
#include "Logger/Logger.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/utilities/fonts/editor_fonts.h"
#include <Windowing/Window/Window.h>

// IMGUI 
// ===================================
#include "ImGuiUtils.h"
#include "imgui.h"
#include <imgui_internal.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL_opengl.h>
// ===================================

namespace SCION_EDITOR
{
bool Gui::InitImGui( SCION_WINDOWING::Window* pWindow )
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

	io.Fonts->AddFontDefault();
	float baseFontSize = 16.0f;
	float iconFontSize = baseFontSize * 2.0f / 3.0f;

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = iconFontSize;
	icons_config.GlyphOffset = ImVec2{ 0.f, 2.f };
	io.Fonts->AddFontFromMemoryTTF( fa_solid_900, fa_solid_900_size, baseFontSize, &icons_config, icons_ranges );

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

void Gui::End( SCION_WINDOWING::Window* pWindow )
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

	ImGuiIO& io = ImGui::GetIO();
	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		SDL_GLContext backupContext = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		SDL_GL_MakeCurrent( pWindow->GetWindow().get(), backupContext );
	}
}

void Gui::ShowImGuiDemo()
{
	ImGui::ShowDemoWindow();
}

} // namespace SCION_EDITOR
