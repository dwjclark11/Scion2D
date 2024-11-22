#include "ImGuiUtils.h"
#include "Logger/Logger.h"

namespace ImGui
{

void InitDefaultStyles()
{
	// TODO: Add this to a custom init file that we can change/adjust easier.
	
	// Get the styles
	ImGuiStyle& style = ImGui::GetStyle();
	
	style.TabRounding = 4.f;
	style.TabBorderSize = 1.f;
	style.FrameBorderSize = 0.1f;
	style.FramePadding.x = 4.f;
	style.FramePadding.y = 3.f;
	style.FrameRounding = 2.f;
	style.ScrollbarSize = 10.f;
	style.ScrollbarRounding = 6.f;
	style.WindowBorderSize = 1.f;
	style.WindowPadding.x = 10.f;
	style.WindowPadding.y = 10.f;
	style.WindowRounding = 8.f;
	style.WindowTitleAlign.x = 0.5f;
	style.ItemSpacing.x = 3.f;
	style.ItemInnerSpacing.x = 0.f;
	style.ItemInnerSpacing.y = 4.f;
	style.ButtonTextAlign.x = 0.5f;
	style.ButtonTextAlign.y = 0.5f;

	// TODO: Adjust colors
}

void ColoredLabel( const std::string& label, const ImVec2& size, const ImVec4& color )
{
	ImGui::PushStyleColor( ImGuiCol_Button, color );
	ImGui::PushStyleColor( ImGuiCol_ButtonHovered, color );
	ImGui::PushStyleColor( ImGuiCol_ButtonActive, color );
	ImGui::Button( label.c_str(), size );
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

void OffsetTextX( const std::string& label, float position )
{
	ImGui::SetCursorPosX( position );
	ImGui::Text( label.c_str() );
}

void AddSpaces( int numSpaces )
{
	SCION_ASSERT( numSpaces > 0 && "Number of spaces must be a positive number!" );
	for ( int i = 0; i < numSpaces; ++i )
		ImGui::Spacing();
}

void InlineLabel( const std::string& label, float spaceSize )
{
	ImGui::Text( label.c_str() );
	ImGui::SameLine();
	ImGui::SetCursorPosX( spaceSize );
}
} // namespace ImGui
