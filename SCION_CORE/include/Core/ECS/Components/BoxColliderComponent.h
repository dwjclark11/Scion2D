#pragma once
#include <glm/glm.hpp>
#include <sol/sol.hpp>

namespace Scion::Core::ECS
{
struct BoxColliderComponent
{
	/* Width of the box collider. */
	int width{ 4 };
	/* Height of the box collider. */
	int height{ 4 };
	/* X/Y offset of the collider based on the TL position of the owner. */
	glm::vec2 offset{ 0.f };
	/* Is the Collider currently colliding? */
	bool bColliding{ false };

	[[nodiscard]] std::string to_string() const;

	static void CreateLuaBoxColliderBind( sol::state& lua );
};
} // namespace Scion::Core::ECS
