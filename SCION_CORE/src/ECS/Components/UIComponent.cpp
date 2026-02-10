#include "Core/ECS/Components/UIComponent.h"
#include <entt/entt.hpp>

namespace Scion::Core::ECS
{
void UIComponent::CreateLuaBind( sol::state& lua )
{
	lua.new_enum<UIObjectType>( "UIObjectType",
								{
									{ "Panel", UIObjectType::PANEL },
									{ "Selector", UIObjectType::SELECTOR },
									{ "PanelLayout", UIObjectType::PANEL_LAYOUT },
									{ "Textbox", UIObjectType::TEXTBOX },
									{ "NoType", UIObjectType::NO_TYPE },
								} );

	lua.new_usertype<UIComponent>( "UIComp",
								   "type_id",
								   entt::type_hash<UIComponent>::value,
								   sol::call_constructor,
								   sol::factories( [] { return UIComponent{}; },
												   []( UIObjectType eType ) { return UIComponent{ .eType = eType }; } ),
								   "type",
								   &UIComponent::eType );
}
} // namespace Scion::Core::ECS
