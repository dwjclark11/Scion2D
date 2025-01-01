#pragma once
#include <sol/sol.hpp>
#include <Rendering/Essentials/Vertex.h>

namespace SCION_CORE::ECS
{
struct TextComponent
{
	/* The name of the font to use. This must exist in the Asset Manager. */
	std::string sFontName{ "pixel" };
	/* The actual string value to be drawn to the screen. */
	std::string sTextStr{ "" };
	/* Any padding to be used around the text. */
	int padding{ 0 };
	/* The length of the text line before it will wrap to the next line. */
	float wrap{ -1.f };
	/* Color of the text. Defaults to white. */
	SCION_RENDERING::Color color{ 255, 255, 255, 255 };
	/* Should the text be drawn or hidden? */
	bool bHidden{ false };

	[[nodiscard]] std::string to_string();

	static void CreateLuaTextBindings( sol::state& lua );
};
} // namespace SCION_CORE::ECS
