#pragma once
#include <glm/glm.hpp>
#include <sol/sol.hpp>

namespace SCION_CORE::ECS
{
struct TransformComponent
{
	/* The X/Y position of the entity in the world. */
	glm::vec2 position{ 0.f };
	/* The X/Y position of the entity based on the position of the parent. */
	glm::vec2 localPosition{ 0.f };
	/* The X/Y values in which to scale the entity. Negative values will flip the sprite. */
	glm::vec2 scale{ 1.f };
	/* The rotation of the entity in degrees. */
	float rotation{ 0.f };

	[[nodiscard]] std::string to_string();

	static void CreateLuaTransformBind( sol::state& lua );
};
} // namespace SCION_CORE::ECS
