#pragma once
#include <imgui.h>
#include <string>
#include <fmt/format.h>

constexpr ImVec4 LABEL_RED = ImVec4{ 0.92f, 0.18f, 0.05f, 1.0f };
constexpr ImVec4 LABEL_GREEN = ImVec4{ 0.05f, 0.93f, 0.25f, 1.f };
constexpr ImVec4 LABEL_BLUE = ImVec4{ 0.05f, 0.18f, 0.91f, 1.f };

constexpr ImVec4 BUTTON_HELD = ImVec4{ 0.f, 0.9f, 0.f, 0.3f };

constexpr ImVec4 BLACK_TRANSPARENT = ImVec4{ 0.f, 0.f, 0.f, 0.f };

constexpr ImVec2 LABEL_SINGLE_SIZE = ImVec2{ 20.f, 20.f };
constexpr ImVec2 TOOL_BUTTON_SIZE = ImVec2{ 32.f, 32.f };

constexpr ImGuiColorEditFlags IMGUI_COLOR_PICKER_FLAGS =
	ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB;

constexpr ImGuiTableFlags IMGUI_NORMAL_TABLE_FLAGS =
	ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;

namespace ImGui
{

void InitDefaultStyles();

void ColoredLabel( const std::string& label, const ImVec2& size = ImVec2{ 32.f, 32.f },
				   const ImVec4& color = ImVec4{ 1.f, 1.f, 1.f, 1.f } );
void OffsetTextX( const std::string& label, float position );
void AddSpaces( int numSpaces );
void InlineLabel( const std::string& label, float spaceSize = 128.f );

void ActiveButton( const char* label, ImVec2 size = TOOL_BUTTON_SIZE );
void DisabledButton( const char* label, ImVec2 size = TOOL_BUTTON_SIZE, const std::string& disabledMsg = "" );

void ActiveImageButton( const char* buttonId, ImTextureID textureID, ImVec2 size = TOOL_BUTTON_SIZE );
void DisabledImageButton( const char* buttonId, ImTextureID textureID, ImVec2 size = TOOL_BUTTON_SIZE,
						  const std::string& disabledMsg = "" );

template <typename... Args>
void ItemToolTip( const std::string_view tipMsg, Args&&... args )
{
	if ( ImGui::IsItemHovered() )
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted( fmt::vformat( tipMsg, fmt::make_format_args( args... ) ).c_str() );
		ImGui::EndTooltip();
	}
}

template <typename... Args>
void TextFormatted( const std::string_view text, Args&&... args )
{
	ImGui::TextUnformatted( fmt::vformat( text, fmt::make_format_args( args... ) ).c_str() );
}

} // namespace ImGui
