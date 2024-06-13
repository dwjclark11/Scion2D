#include "Core/ECS/Components/CircleColliderComponent.h"
#include <entt/entt.hpp>

std::string SCION_CORE::ECS::CircleColliderComponent::to_string() const
{
	std::stringstream ss;
	ss << "==== Circle Collider Component ==== \n"
	   << std::boolalpha << "Radius: " << radius << "\n"
	   << "Offset: [ x = " << offset.x << ", y = " << offset.y << "]"
	   << "\n";

	return ss.str();
}

void SCION_CORE::ECS::CircleColliderComponent::CreateLuaCircleColliderBind( sol::state& lua )
{
	lua.new_usertype<CircleColliderComponent>(
		"CircleCollider",
		"type_id",
		entt::type_hash<CircleColliderComponent>::value,
		sol::call_constructor,
		sol::factories( []( float radius ) { return CircleColliderComponent{ .radius = radius }; },
						[]( float radius, glm::vec2 offset ) {
							return CircleColliderComponent{ .radius = radius, .offset = offset };
						} ),
		"radius",
		&CircleColliderComponent::radius,
		"offset",
		&CircleColliderComponent::offset,
		"bColliding",
		&CircleColliderComponent::bColliding );
}
