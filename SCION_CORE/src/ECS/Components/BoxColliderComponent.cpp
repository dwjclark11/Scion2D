#include "Core/ECS/Components/BoxColliderComponent.h"
#include <entt/entt.hpp>

std::string Scion::Core::ECS::BoxColliderComponent::to_string() const
{
	std::stringstream ss;
	ss << "==== Box Collider Component ==== \n"
	   << std::boolalpha << "Width: " << width << "\n"
	   << "Height: " << height << "\n"
	   << "Offset: [ x = " << offset.x << ", y = " << offset.y << "]"
	   << "\n";

	return ss.str();
}

void Scion::Core::ECS::BoxColliderComponent::CreateLuaBoxColliderBind( sol::state& lua )
{
	lua.new_usertype<BoxColliderComponent>( "BoxCollider",
											"type_id",
											entt::type_hash<BoxColliderComponent>::value,
											sol::call_constructor,
											sol::factories( []( int width, int height, glm::vec2 offset ) {
												return BoxColliderComponent{
													.width = width, .height = height, .offset = offset };
											} ),
											"width",
											&BoxColliderComponent::width,
											"height",
											&BoxColliderComponent::height,
											"offset",
											&BoxColliderComponent::offset,
											"bColliding",
											&BoxColliderComponent::bColliding );
}
