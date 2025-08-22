#include "EditorStyleToolDisplay.h"
#include <imgui.h>

namespace SCION_EDITOR
{

void EditorStyleToolDisplay::DrawSizePicker()
{
	if ( !ImGui::Begin( "Editor Size Styles" ) )
	{
		ImGui::End();
		return;
	}

	ImGui::PushItemWidth( 250.f );
	// Get the styles
	ImGuiStyle& style = ImGui::GetStyle();
	if ( ImGui::InputFloat( "Tab Rounding", &style.TabRounding, 1.f, 1.f, "%.1f" ) )
		style.TabRounding = std::clamp( style.TabRounding, 0.f, 10.f );

	if ( ImGui::InputFloat( "Tab Border Size", &style.TabBorderSize, 1.f, 1.f, "%.1f" ) )
		style.TabBorderSize = std::clamp( style.TabBorderSize, 0.f, 10.f );

	if ( ImGui::InputFloat( "Frame Border Size", &style.FrameBorderSize, 0.1f, 0.1f, "%.1f" ) )
		style.FrameBorderSize = std::clamp( style.FrameBorderSize, 0.f, 1.f );

	if ( ImGui::InputFloat( "Frame Padding Width", &style.FramePadding.x, 1.f, 1.f, "%.1f" ) )
		style.FramePadding.x = std::clamp( style.FramePadding.x, 0.f, 10.f );

	if ( ImGui::InputFloat( "Frame Padding Height", &style.FramePadding.y, 1.f, 1.f, "%.1f" ) )
		style.FramePadding.y = std::clamp( style.FramePadding.y, 0.f, 10.f );

	if ( ImGui::InputFloat( "Frame Rounding", &style.FrameRounding, 1.f, 1.f, "%.1f" ) )
		style.FrameRounding = std::clamp( style.FrameRounding, 0.f, 10.f );

	if ( ImGui::InputFloat( "Scrollbar Size", &style.ScrollbarSize, 1.f, 1.f, "%.1f" ) )
		style.ScrollbarSize = std::clamp( style.ScrollbarSize, 1.f, 10.f );

	if ( ImGui::InputFloat( "Scrollbar Rounding", &style.ScrollbarRounding, 1.f, 1.f, "%.1f" ) )
		style.ScrollbarRounding = std::clamp( style.ScrollbarRounding, 0.f, 10.f );

	if ( ImGui::InputFloat( "Window Border Size", &style.WindowBorderSize, .1f, .1f, "%.1f" ) )
		style.WindowBorderSize = std::clamp( style.ScrollbarRounding, 0.f, 1.f );

	if ( ImGui::InputFloat( "Window Padding Width", &style.WindowPadding.x, 1.f, 1.f, "%.1f" ) )
		style.WindowPadding.x = std::clamp( style.WindowPadding.x, 0.f, 10.f );

	if ( ImGui::InputFloat( "Window Padding Height", &style.WindowPadding.y, 1.f, 1.f, "%.1f" ) )
		style.WindowPadding.y = std::clamp( style.WindowPadding.y, 0.f, 10.f );

	if ( ImGui::InputFloat( "Window Rounding", &style.WindowRounding, 1.f, 1.f, "%.1f" ) )
		style.WindowRounding = std::clamp( style.WindowPadding.y, 0.f, 10.f );

	if ( ImGui::InputFloat( "Window Title Align X pos", &style.WindowTitleAlign.x, .1f, .1f, "%.1f" ) )
		style.WindowTitleAlign.x = std::clamp( style.WindowTitleAlign.x, 0.f, 1.f );

	if ( ImGui::InputFloat( "Item Spacing X pos", &style.ItemSpacing.x, 1.f, 1.f, "%.1f" ) )
		style.ItemSpacing.x = std::clamp( style.ItemSpacing.x, 0.f, 10.f );

	if ( ImGui::InputFloat( "Item Inner Spacing X", &style.ItemInnerSpacing.x, .1f, .1f, "%.1f" ) )
		style.ItemInnerSpacing.x = std::clamp( style.ItemInnerSpacing.x, 0.f, 10.f );

	if ( ImGui::InputFloat( "Item Inner Spacing Y", &style.ItemInnerSpacing.y, .1f, .1f, "%.1f" ) )
		style.ItemInnerSpacing.y = std::clamp( style.ItemInnerSpacing.y, 0.f, 10.f );

	if ( ImGui::InputFloat( "Button Text Align X pos", &style.ButtonTextAlign.x, .1f, .1f, "%.1f" ) )
		style.ButtonTextAlign.x = std::clamp( style.ButtonTextAlign.x, 0.f, 1.f );

	if ( ImGui::InputFloat( "Button Text Align Y pos", &style.ButtonTextAlign.y, .1f, .1f, "%.1f" ) )
		style.ButtonTextAlign.y = std::clamp( style.ButtonTextAlign.y, 0.f, 1.f );
	ImGui::PopItemWidth();

	ImGui::End();
}

void EditorStyleToolDisplay::DrawColorPicker()
{
	if ( !ImGui::Begin( "Editor Color Styles" ) )
	{
		ImGui::End();
		return;
	}
	ImGui::PushItemWidth( 250.f );
	ImVec4* colors = ImGui::GetStyle().Colors;
	ImGui::Text( "Style Colors" );
	ImGui::ColorEdit4( "Text", &colors[ ImGuiCol_Text ].x );
	ImGui::ColorEdit4( "Disabled Text", &colors[ ImGuiCol_TextDisabled ].x );
	ImGui::ColorEdit4( "Window BG", &colors[ ImGuiCol_WindowBg ].x );
	ImGui::ColorEdit4( "Child BG", &colors[ ImGuiCol_ChildBg ].x );
	ImGui::ColorEdit4( "Popup BG", &colors[ ImGuiCol_PopupBg ].x );
	ImGui::ColorEdit4( "Border BG", &colors[ ImGuiCol_Border ].x );
	ImGui::ColorEdit4( "Border Shadow", &colors[ ImGuiCol_BorderShadow ].x );
	ImGui::ColorEdit4( "Frame BG", &colors[ ImGuiCol_FrameBg ].x );
	ImGui::ColorEdit4( "Frame BG Hovered", &colors[ ImGuiCol_FrameBgHovered ].x );
	ImGui::ColorEdit4( "Frame BG Active", &colors[ ImGuiCol_FrameBgActive ].x );
	ImGui::ColorEdit4( "Title BG", &colors[ ImGuiCol_TitleBg ].x );
	ImGui::ColorEdit4( "Title BG Active", &colors[ ImGuiCol_TitleBgActive ].x );
	ImGui::ColorEdit4( "Title BG Collapsed", &colors[ ImGuiCol_TitleBgCollapsed ].x );
	ImGui::ColorEdit4( "Menu Bar BG", &colors[ ImGuiCol_MenuBarBg ].x );
	ImGui::ColorEdit4( "ScrollBar BG", &colors[ ImGuiCol_ScrollbarBg ].x );
	ImGui::ColorEdit4( "ScrollBar Grab", &colors[ ImGuiCol_ScrollbarGrab ].x );
	ImGui::ColorEdit4( "ScrollBar Grab Hovered", &colors[ ImGuiCol_ScrollbarGrabHovered ].x );
	ImGui::ColorEdit4( "ScrollBar Grab Active", &colors[ ImGuiCol_ScrollbarGrabActive ].x );
	ImGui::ColorEdit4( "Checkmark", &colors[ ImGuiCol_CheckMark ].x );
	ImGui::ColorEdit4( "Slider Grab", &colors[ ImGuiCol_SliderGrab ].x );
	ImGui::ColorEdit4( "Slider Grab Active", &colors[ ImGuiCol_SliderGrabActive ].x );
	ImGui::ColorEdit4( "Button", &colors[ ImGuiCol_Button ].x );
	ImGui::ColorEdit4( "Button Hovered", &colors[ ImGuiCol_ButtonHovered ].x );
	ImGui::ColorEdit4( "Button Active", &colors[ ImGuiCol_ButtonActive ].x );
	ImGui::ColorEdit4( "Header", &colors[ ImGuiCol_Header ].x );
	ImGui::ColorEdit4( "Header Hovered", &colors[ ImGuiCol_HeaderHovered ].x );
	ImGui::ColorEdit4( "Header Active", &colors[ ImGuiCol_HeaderActive ].x );
	ImGui::ColorEdit4( "Separator", &colors[ ImGuiCol_Separator ].x );
	ImGui::ColorEdit4( "Separator Hovered", &colors[ ImGuiCol_SeparatorHovered ].x );
	ImGui::ColorEdit4( "Separator Active", &colors[ ImGuiCol_SeparatorActive ].x );
	ImGui::ColorEdit4( "Resize Grip", &colors[ ImGuiCol_ResizeGrip ].x );
	ImGui::ColorEdit4( "Resize Grip Hovered", &colors[ ImGuiCol_ResizeGripHovered ].x );
	ImGui::ColorEdit4( "Resize Grip Active", &colors[ ImGuiCol_ResizeGripActive ].x );
	ImGui::ColorEdit4( "Tab", &colors[ ImGuiCol_Tab ].x );
	ImGui::ColorEdit4( "Tab Hovered", &colors[ ImGuiCol_TabHovered ].x );
	ImGui::ColorEdit4( "Tab Active", &colors[ ImGuiCol_TabActive ].x );
	ImGui::ColorEdit4( "Tab Unfocused", &colors[ ImGuiCol_TabUnfocused ].x );
	ImGui::ColorEdit4( "Tab Unfocused Active", &colors[ ImGuiCol_TabUnfocusedActive ].x );
	ImGui::ColorEdit4( "Docking Preview", &colors[ ImGuiCol_DockingPreview ].x );
	ImGui::ColorEdit4( "Docking Empty BG", &colors[ ImGuiCol_DockingEmptyBg ].x );
	ImGui::ColorEdit4( "Plot Lines", &colors[ ImGuiCol_PlotLines ].x );
	ImGui::ColorEdit4( "Plot Lines Hovered", &colors[ ImGuiCol_PlotLinesHovered ].x );
	ImGui::ColorEdit4( "Plot Histogram", &colors[ ImGuiCol_PlotHistogram ].x );
	ImGui::ColorEdit4( "Plot Histogram Hovered", &colors[ ImGuiCol_PlotHistogramHovered ].x );
	ImGui::ColorEdit4( "Text Selected BG", &colors[ ImGuiCol_TextSelectedBg ].x );
	ImGui::ColorEdit4( "Drag Drop Target", &colors[ ImGuiCol_DragDropTarget ].x );
	ImGui::ColorEdit4( "Nav Highlight", &colors[ ImGuiCol_NavHighlight ].x );
	ImGui::ColorEdit4( "Nav Window Highlight", &colors[ ImGuiCol_NavWindowingHighlight ].x );
	ImGui::ColorEdit4( "Nav Window Dim BG", &colors[ ImGuiCol_NavWindowingDimBg ].x );
	ImGui::ColorEdit4( "Modal Window Dim BG", &colors[ ImGuiCol_ModalWindowDimBg ].x );

	ImGui::PopItemWidth();
	ImGui::End();
}

EditorStyleToolDisplay::EditorStyleToolDisplay()
{
}

void EditorStyleToolDisplay::Draw()
{
	DrawSizePicker();
	DrawColorPicker();
}

} // namespace SCION_EDITOR
