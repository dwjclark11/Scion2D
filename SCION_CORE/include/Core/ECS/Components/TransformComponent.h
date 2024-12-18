#pragma once
#include <glm/glm.hpp>
#include <sol/sol.hpp>

namespace SCION_CORE::ECS
{
struct TransformComponent
{
	glm::vec2 position{ 0.f };
	glm::vec2 scale{ glm::vec2{ 1.f } };
	float rotation{ 0.f };

	[[nodiscard]] std::string to_string();

	static void CreateLuaTransformBind( sol::state& lua );
};
} // namespace SCION_CORE::ECS
