#pragma once
#include <sol/sol.hpp>
#include <Rendering/Essentials/Vertex.h>

namespace Scion::Core::ECS
{
struct TextComponent
{
	/* The name of the font to use. This must exist in the Asset Manager. Defaults to pixel-32. */
	std::string sFontName{ "pixel-32" };
	/* The actual string value to be drawn to the screen. */
	std::string sTextStr{};
	/* Any padding to be used around the text. */
	int padding{ 0 };
	/* The length of the text line before it will wrap to the next line. */
	float wrap{ -1.f };
	/* The width of the containing text box. */
	float textBoxWidth{ -1.f };
	/* The height of the containing text box. */
	float textBoxHeight{ -1.f };
	/* Color of the text. Defaults to white. */
	Scion::Rendering::Color color{ 255, 255, 255, 255 };
	/* Should the text be drawn or hidden? */
	bool bHidden{ false };
	/* Text Component has been changed, sizes need to be updated. */
	bool bDirty{ false };

	[[nodiscard]] std::string to_string();

	static void CreateLuaTextBindings( sol::state& lua );
};
} // namespace Scion::Core::ECS
