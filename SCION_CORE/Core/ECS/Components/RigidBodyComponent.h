#pragma once
#include <glm/glm.hpp>
#include <sol/sol.hpp>

namespace SCION_CORE::ECS {
	struct RigidBodyComponent
	{
		glm::vec2 velocity{0.f};

		static void CreateRigidBodyBind(sol::state& lua);
	};
}