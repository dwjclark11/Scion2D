#pragma once
#include <imgui.h>
#include <string>

constexpr ImVec4 LABEL_RED = ImVec4{ 0.92f, 0.18f, 0.05f, 1.0f };
constexpr ImVec4 LABEL_GREEN = ImVec4{ 0.05f, 0.93f, 0.25f, 1.f };
constexpr ImVec4 LABEL_BLUE = ImVec4{ 0.05f, 0.18f, 0.91f, 1.f };

constexpr ImVec2 LABEL_SINGLE_SIZE = ImVec2{ 20.f, 20.f };

constexpr ImGuiColorEditFlags IMGUI_COLOR_PICKER_FLAGS =
	ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB;

constexpr ImGuiTableFlags IMGUI_NORMAL_TABLE_FLAGS =
	ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;

namespace ImGui
{
void ColoredLabel( const std::string& label, const ImVec2& size = ImVec2{ 32.f, 32.f },
				   const ImVec4& color = ImVec4{ 1.f, 1.f, 1.f, 1.f } );
void OffsetTextX( const std::string& label, float position );
void AddSpaces( int numSpaces );
void InlineLabel( const std::string& label, float spaceSize = 128.f );

} // namespace ImGui
