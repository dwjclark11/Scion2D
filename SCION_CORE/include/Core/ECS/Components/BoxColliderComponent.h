#pragma once
#include <glm/glm.hpp>
#include <sol/sol.hpp>

namespace SCION_CORE::ECS {
	struct BoxColliderComponent
	{
		int width{ 0 }, height{ 0 };
		glm::vec2 offset{glm::vec2{0}};
		bool bColliding{ false };

		[[nodiscard]] std::string to_string() const;

		static void CreateLuaBoxColliderBind(sol::state& lua);
	};
}