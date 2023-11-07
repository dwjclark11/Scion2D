#include "BoxColliderComponent.h"
#include <entt.hpp>
void SCION_CORE::ECS::BoxColliderComponent::CreateLuaBoxColliderBind(sol::state& lua)
{
	lua.new_usertype<BoxColliderComponent>(
		"BoxCollider",
		"type_id", entt::type_hash<BoxColliderComponent>::value,
		sol::call_constructor,
		sol::factories(
			[](int width, int height, glm::vec2 offset) {
				return BoxColliderComponent{ .width = width, .height = height, .offset = offset };
			}
		),
		"width", &BoxColliderComponent::width,
		"height", &BoxColliderComponent::height,
		"offset", &BoxColliderComponent::offset,
		"bColliding", &BoxColliderComponent::bColliding
	);
}
