#pragma once
#include <glm/glm.hpp>
#include <sol/sol.hpp>

namespace SCION_CORE::ECS
{
/*
 * Rigid Body Component - Use this when you don't want the physics to be controlled by
 * Box2D
 */
struct RigidBodyComponent
{
	/* The current velocity the the body is moving at. */
	glm::vec2 currentVelocity{ 0.f };
	/* The max velocity to clamp the object. */
	glm::vec2 maxVelocity{ 0.f };

	[[nodiscard]] std::string to_string() const;

	static void CreateRigidBodyBind( sol::state& lua );
};
} // namespace SCION_CORE::ECS
