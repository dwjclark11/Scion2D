#include "RigidBodyComponent.h"
#include <entt.hpp>

void SCION_CORE::ECS::RigidBodyComponent::CreateRigidBodyBind(sol::state& lua)
{
	lua.new_usertype<RigidBodyComponent>(
		"RigidBody",
		"type_id", entt::type_hash<RigidBodyComponent>::value,
		sol::call_constructor,
		sol::factories(
			[](const glm::vec2& velocity) {
				return RigidBodyComponent{ .velocity = velocity };
			}
		),
		"velocity", &RigidBodyComponent::velocity
	);
}
