#pragma once
#include <sol/sol.hpp>
#include <Rendering/Essentials/Vertex.h>

namespace SCION_CORE::ECS {
	struct TextComponent
	{
		std::string sFontName{"pixel"}, sTextStr{ "" };
		int padding{ 0 };
		float wrap{ -1.f };
		SCION_RENDERING::Color color { 255, 255, 255, 255 };
		bool bHidden{ false };

		[[nodiscard]] std::string to_string();

		static void CreateLuaTextBindings(sol::state& lua);
	};
}