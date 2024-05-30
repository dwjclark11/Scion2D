#pragma once
#include "Core/ECS/Registry.h"

namespace SCION_CORE::Systems
{
class PhysicsSystem
{
  private:
	SCION_CORE::ECS::Registry& m_Registry;

  public:
	PhysicsSystem(SCION_CORE::ECS::Registry& registry);
	~PhysicsSystem() = default;

	void Update(entt::registry& registry);
};
} // namespace SCION_CORE::Systems