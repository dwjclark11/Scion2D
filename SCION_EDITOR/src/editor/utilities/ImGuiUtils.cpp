#include "ImGuiUtils.h"
#include "Logger/Logger.h"

namespace ImGui
{
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
