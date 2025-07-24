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

/**
 * @brief Initializes a dark-themed default style for ImGui.
 *
 * Sets base colors, rounding, padding, spacing, and scaling for the ImGui interface.
 */
void InitDefaultStyles();

/**
 * @brief Renders a colored label using a styled ImGui button.
 *
 * The label appears as a button colored with the given RGBA color.
 *
 * @param label Text to display.
 * @param size Size of the button.
 * @param color Color used for all button states.
 */
void ColoredLabel( const std::string& label, const ImVec2& size = ImVec2{ 32.f, 32.f },
				   const ImVec4& color = ImVec4{ 1.f, 1.f, 1.f, 1.f } );

/**
 * @brief Offsets the horizontal position of the text within the current ImGui window.
 *
 * Useful for aligning text with custom horizontal positioning.
 *
 * @param label Text to display.
 * @param position Horizontal position (in pixels) relative to window origin.
 */
void OffsetTextX( const std::string& label, float position );

/**
 * @brief Adds vertical spacing elements.
 *
 * Inserts a given number of vertical spaces using ImGui::Spacing().
 *
 * @param numSpaces Number of spacing calls to insert.
 */
void AddSpaces( int numSpaces );

/**
 * @brief Displays a label followed by space on the same line.
 *
 * Moves the cursor to a specified horizontal offset after rendering the label.
 *
 * @param label Text to display.
 * @param spaceSize X offset to move the cursor to after the label.
 */
void InlineLabel( const std::string& label, float spaceSize = 128.f );

/**
 * @brief Displays a button with a consistent active color for all states.
 *
 * Styled using the BUTTON_HELD color for all button interactions.
 *
 * @param label Text to display on the button.
 * @param size Button dimensions.
 */
void ActiveButton( const char* label, ImVec2 size = TOOL_BUTTON_SIZE );


/**
 * @brief Displays a disabled button with optional tooltip.
 *
 * Button is non-interactive, and shows a tooltip if provided.
 *
 * @param label Text to display on the button.
 * @param size Button dimensions.
 * @param disabledMsg Tooltip to show when hovering (optional).
 */
void DisabledButton( const char* label, ImVec2 size = TOOL_BUTTON_SIZE, const std::string& disabledMsg = "" );

/**
 * @brief Displays an image button with a consistent active style.
 *
 * Applies the BUTTON_HELD color to all interaction states.
 *
 * @param buttonId Unique ID for the button.
 * @param textureID Texture to display.
 * @param size Dimensions of the image.
 */
void ActiveImageButton( const char* buttonId, ImTextureID textureID, ImVec2 size = TOOL_BUTTON_SIZE );

/**
 * @brief Displays a disabled image button with optional tooltip.
 *
 * Image button is non-interactive, and shows a tooltip if provided.
 *
 * @param buttonId Unique ID for the button.
 * @param textureID Texture to display.
 * @param size Dimensions of the image.
 * @param disabledMsg Tooltip to show when hovering (optional).
 */
void DisabledImageButton( const char* buttonId, ImTextureID textureID, ImVec2 size = TOOL_BUTTON_SIZE,
						  const std::string& disabledMsg = "" );

/**
 * @brief Displays a read-only input text field with dimmed styling.
 *
 * Visually indicates the field is non-editable.
 *
 * @param sLabel Label for the input field.
 * @param sInputText Pointer to the text data (will not be modified).
 */
void InputTextReadOnly( const std::string& sLabel, std::string* sInputText );

// Font Helpers

/**
 * @brief Retrieves a loaded ImFont* by its name.
 *
 * If the font is not found, returns the default ImGui font and logs an error.
 *
 * @param sFontName Name of the font to retrieve.
 * @return Pointer to the requested ImFont, or default font if not found.
 */
ImFont* GetFont( const std::string& sFontName );

/**
 * @brief Adds a new ImFont* to the font map under a given name.
 *
 * Ensures the font name does not already exist and the pointer is valid.
 *
 * @param sFontName Name to associate with the font.
 * @param pFont Pointer to the ImFont object to add.
 * @param fontSize Size of the font (not used internally).
 * @return true if the font was added successfully; false otherwise.
 */
bool AddFont( const std::string& sFontName, ImFont* pFont, float fontSize );

/**
 * @brief Loads a font from a TTF file and registers it in the font map.
 *
 * Adds the font using the specified name and size if it does not already exist.
 *
 * @param sFontName Name to associate with the loaded font.
 * @param sFontFile Path to the font file (.ttf).
 * @param fontSize Desired font size.
 * @return true if the font was loaded and added successfully; false otherwise.
 */
bool AddFontFromFile( const std::string sFontName, const std::string& sFontFile, float fontSize );

/**
 * @brief Loads a font from memory and registers it in the font map.
 *
 * The font data must be a valid TTF memory block. Adds the font only if it does not already exist.
 *
 * @param sFontName Name to associate with the font.
 * @param fontData Pointer to the TTF font data in memory.
 * @param dataSize Size of the font data in bytes.
 * @param fontSize Desired font size.
 * @return true if the font was added successfully from memory; false otherwise.
 */
bool AddFontFromMemory( const std::string& sFontName, void* fontData, float dataSize, float fontSize );

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
