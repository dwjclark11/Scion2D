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
	glm::vec2 velocity{0.f};
	// TODO: Add more members as needed

	[[nodiscard]] std::string to_string();

	static void CreateRigidBodyBind(sol::state& lua);
};
} // namespace SCION_CORE::ECS