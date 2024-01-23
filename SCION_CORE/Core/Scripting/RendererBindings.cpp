#include "RendererBindings.h"
#include <Rendering/Essentials/Primitives.h>
#include <Rendering/Core/Camera2D.h>
#include <Rendering/Core/Renderer.h>
#include "../ECS/Registry.h"
#include <Logger/Logger.h>

using namespace SCION_RENDERING;

void SCION_CORE::Scripting::RendererBinder::CreateRenderingBind(sol::state& lua, SCION_CORE::ECS::Registry& registry)
{
	// Primitives Bind
	lua.new_usertype<Line>(
		"Line",
		sol::call_constructor,
		sol::factories(
			[](const glm::vec2& p1, const glm::vec2& p2, const Color& color)
			{
				return Line{ .p1 = p1, .p2 = p2, .color = color };
			}
		),
		"p1", &Line::p1,
		"p2", &Line::p2,
		"color", &Line::color
	);

	lua.new_usertype<Rect>(
		"Rect",
		sol::call_constructor,
		sol::factories(
			[](const glm::vec2& position, float width, float height, const Color& color)
			{
				return Rect{ .position = position, .width = width, .height = height, .color = color };
			}
		),
		"position", &Rect::position,
		"width", &Rect::width,
		"height", &Rect::height,
		"color", &Rect::color
	);

	lua.new_usertype<Circle>(
		"Circle",
		sol::call_constructor,
		sol::factories(
			[](const glm::vec2& position, float lineThickness, float radius, const Color& color)
			{
				return Circle{ .position = position, .lineThickness = lineThickness, .radius = radius, .color = color };
			}
		),
		"position", &Circle::position,
		"lineThickness", &Circle::lineThickness,
		"radius", &Circle::radius,
		"color", &Circle::color
	);

	// Bind the renderer
	auto& renderer = registry.GetContext<std::shared_ptr<Renderer>>();
	if (!renderer)
	{
		SCION_ERROR("Failed to bind the Renderer to Lua! Not in the registry context!");
		return;
	}

	lua.set_function(
		"DrawRect", sol::overload(
			[&](const Rect& rect) {
				renderer->DrawRect(rect);
			},
			[&](const glm::vec2& position, float width, float height, const Color& color) {
				renderer->DrawRect(position, width, height, color);
			}
		)
	);

	lua.set_function(
		"DrawLine", sol::overload(
			[&](const Line& line) {
				renderer->DrawLine(line);
			},
			[&](const glm::vec2& p1, const glm::vec2& p2, const Color& color) {
				renderer->DrawLine(p1, p2, color);
			}
		)
	);

	lua.set_function(
		"DrawCircle", sol::overload(
			[&](const Circle& circle) {
				renderer->DrawCircle(circle);
			},
			[&](const glm::vec2& pos, float lineThickness, float radius, const Color& color) {
				renderer->DrawCircle(pos, radius, color, lineThickness);
			}
		)
	);

	lua.set_function(
		"DrawFilledRect", [&](const Rect& rect) {
			renderer->DrawFilledRect(rect);
		}
	);

	auto& camera = registry.GetContext<std::shared_ptr<Camera2D>>();
	if (!camera)
	{
		SCION_ERROR("Failed to bind the camera to lua - Failed to get the camera from the registry context!");
		return;
	}

	// Bind The Camera!
	lua.new_usertype<Camera2D>(
		"Camera",
		sol::no_constructor,
		"get", [&] {
			return *camera;
		},
		"position", [&] {
			return camera->GetPosition();
		},
		"set_position", [&](const glm::vec2 newPosition) {
			camera->SetPosition(newPosition);
		},
		"scale", [&] {
			return camera->GetScale();
		},
		"set_scale", [&](float scale) {
			camera->SetScale(scale);
		}
	);
}
