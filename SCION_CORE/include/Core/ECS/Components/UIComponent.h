#pragma once
#include <sol/sol.hpp>

namespace Scion::Core::ECS
{

enum class UIObjectType
{
	PANEL,
	SELECTOR,
	PANEL_LAYOUT,
	TEXTBOX,
	NO_TYPE
};


struct UIComponent
{
	UIObjectType eType{ UIObjectType::NO_TYPE };

	static void CreateLuaBind( sol::state& lua );
};


} // namespace Scion::Core::ECS
