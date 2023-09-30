#include "TransformComponent.h"
#include <entt.hpp>

void SCION_CORE::ECS::TransformComponent::CreateLuaTransformBind(sol::state& lua)
{
	lua.new_usertype<TransformComponent>(
		"Transform",
		"type_id", &entt::type_hash<TransformComponent>::value,
		sol::call_constructor,
		sol::factories(
			[](glm::vec2 position, glm::vec2 scale, float rotation) {
				return TransformComponent{
					.position = position,
					.scale = scale,
					.rotation = rotation
				};
			},
			[](float x, float y, float scale_x, float scale_y, float rotation) {
				return TransformComponent{
					.position = glm::vec2{x, y},
					.scale = glm::vec2{scale_x, scale_y},
					.rotation = rotation
				};
			}
		), 
		"position", &TransformComponent::position,
		"scale", &TransformComponent::scale,
		"rotation", &TransformComponent::rotation
	);
}
