#include "EditorCoreLuaWrappers.h"
#include "Core/Scripting/InputManager.h"
#include "Core/ECS/Registry.h"
#include "Rendering/Core/Camera2D.h"
#include "editor/utilities/EditorUtilities.h"
#include "Logger/Logger.h"

#include <glm/glm.hpp>
#include <imgui.h>
#include <cmath>

namespace SCION_EDITOR
{
void LuaCoreBinder::CreateLuaBind( sol::state& lua, SCION_CORE::ECS::Registry& registry )
{
	auto& mouse = INPUT_MANAGER().GetMouse();
	auto& camera = registry.GetContext<std::shared_ptr<SCION_RENDERING::Camera2D>>();

	lua.new_usertype<Mouse>(
		"Mouse",
		sol::no_constructor,
		"just_pressed",
		[ & ]( int btn ) { return mouse.IsBtnJustPressed( btn ); },
		"just_released",
		[ & ]( int btn ) { return mouse.IsBtnJustReleased( btn ); },
		"pressed",
		[ & ]( int btn ) { return mouse.IsBtnPressed( btn ); },
		"screen_position",
		[ & ]() {
			const auto& mouseInfo = registry.GetContext<std::shared_ptr<MouseGuiInfo>>();
			// If the mouse info is invalid, return the mouse screen position from SDL.
			// This does not take into account ImGui::Docking, windowsize, relative position, etc.
			if (!mouseInfo)
			{
				auto [ x, y ] = mouse.GetMouseScreenPosition();
				return glm::vec2{ x, y };
			}

			return mouseInfo->position;
		},
		"world_position",
		[ & ]() {
			const auto& mouseInfo = registry.GetContext<std::shared_ptr<MouseGuiInfo>>();
			// If the mouse info is invalid, return the mouse screen position from SDL.
			// This does not take into account ImGui::Docking, windowsize, relative position, etc.
			if (!mouseInfo)
			{
				auto [ x, y ] = mouse.GetMouseScreenPosition();
				return camera->ScreenCoordsToWorld( glm::vec2{ x, y } );
			}

			float widthRatio = mouseInfo->windowSize.x / static_cast<float>(camera->GetWidth());
			float heightRatio = mouseInfo->windowSize.y / static_cast<float>(camera->GetHeight());
			SCION_ASSERT( widthRatio > 0 && heightRatio > 0 && "Ensure the width and heigt ratios are above zero!" );
			int x = mouseInfo->position.x / widthRatio;
			int y = mouseInfo->position.y / heightRatio;

			return camera->ScreenCoordsToWorld( glm::vec2{ x, y } );
		},
		"wheel_x",
		[ & ]() { return mouse.GetMouseWheelX(); },
		"wheel_y",
		[ & ]() { return mouse.GetMouseWheelY(); } );

	/*
	* TODO: Add more editor specific Core bindings here.
	*/
}
} // namespace SCION_EDITOR
