#pragma once
#include <glm/glm.hpp>
#include <sol/sol.hpp>

namespace Scion::Core::ECS
{
struct CircleColliderComponent
{
	/* Radius of the circle collider. */
	float radius{ 4.f };
	/* X/Y offset of the collider based on the TL position of the owner. */
	glm::vec2 offset{ 0.f };
	/* Is the Collider currently colliding? */
	bool bColliding{ false };

	[[nodiscard]] std::string to_string() const;

	static void CreateLuaCircleColliderBind( sol::state& lua );
};
} // namespace Scion::Core::ECS
