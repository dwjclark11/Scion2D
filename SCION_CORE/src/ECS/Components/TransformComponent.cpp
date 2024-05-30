#include "Core/ECS/Components/TransformComponent.h"
#include <entt/entt.hpp>

std::string SCION_CORE::ECS::TransformComponent::to_string()
{
	std::stringstream ss;
	ss << "==== Transform Component ==== \n"
	   << "Position: [ x = " << position.x << ", y = " << position.y << "]"
	   << "\n"
	   << "Scale: [ x = " << scale.x << ", y = " << scale.y << "]"
	   << "\n"
	   << "Rotation: " << rotation << "\n";

	return ss.str();
}

void SCION_CORE::ECS::TransformComponent::CreateLuaTransformBind(sol::state& lua)
{
	lua.new_usertype<TransformComponent>(
		"Transform",
		"type_id",
		&entt::type_hash<TransformComponent>::value,
		sol::call_constructor,
		sol::factories(
			[](glm::vec2 position, glm::vec2 scale, float rotation) {
				return TransformComponent{.position = position, .scale = scale, .rotation = rotation};
			},
			[](float x, float y, float scale_x, float scale_y, float rotation) {
				return TransformComponent{
					.position = glm::vec2{x, y}, .scale = glm::vec2{scale_x, scale_y}, .rotation = rotation};
			}),
		"position",
		&TransformComponent::position,
		"scale",
		&TransformComponent::scale,
		"rotation",
		&TransformComponent::rotation,
		"to_string",
		&TransformComponent::to_string);
}
