#include "Core/ECS/Components/RigidBodyComponent.h"
#include <entt/entt.hpp>

std::string SCION_CORE::ECS::RigidBodyComponent::to_string()
{
	std::stringstream ss;
	ss << "==== RigidBody Component ==== \n"
	   << "Velocity: [ x = " << velocity.x << ", y = " << velocity.y << "\n";

	return ss.str();
}

void SCION_CORE::ECS::RigidBodyComponent::CreateRigidBodyBind( sol::state& lua )
{
	lua.new_usertype<RigidBodyComponent>(
		"RigidBody",
		"type_id",
		entt::type_hash<RigidBodyComponent>::value,
		sol::call_constructor,
		sol::factories( []( const glm::vec2& velocity ) { return RigidBodyComponent{ .velocity = velocity }; } ),
		"velocity",
		&RigidBodyComponent::velocity,
		"to_string",
		&RigidBodyComponent::to_string );
}
