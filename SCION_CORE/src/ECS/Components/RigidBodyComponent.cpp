#include "Core/ECS/Components/RigidBodyComponent.h"
#include <entt/entt.hpp>

std::string Scion::Core::ECS::RigidBodyComponent::to_string() const
{
	std::stringstream ss;
	ss << "==== RigidBody Component ==== \n"
	   << "Max Velocity: [ x = " << maxVelocity.x << ", y = " << maxVelocity.y << "\n";

	return ss.str();
}

void Scion::Core::ECS::RigidBodyComponent::CreateRigidBodyBind( sol::state& lua )
{
	lua.new_usertype<RigidBodyComponent>(
		"RigidBody",
		"type_id",
		entt::type_hash<RigidBodyComponent>::value,
		sol::call_constructor,
		sol::factories( []( const glm::vec2& velocity ) { return RigidBodyComponent{ .maxVelocity = velocity }; } ),
		"currentVelocity",
		&RigidBodyComponent::currentVelocity,
		"maxVelocity",
		&RigidBodyComponent::maxVelocity,
		"to_string",
		&RigidBodyComponent::to_string );
}
