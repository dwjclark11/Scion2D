#pragma once
#include <glm/glm.hpp>
#include <sol/sol.hpp>

namespace SCION_CORE::ECS
{
struct CircleColliderComponent
{
	float radius{ 4.f };
	glm::vec2 offset{ 0.f };
	bool bColliding{ false };

	[[nodiscard]] std::string to_string() const;

	static void CreateLuaCircleColliderBind( sol::state& lua );
};
} // namespace SCION_CORE::ECS
