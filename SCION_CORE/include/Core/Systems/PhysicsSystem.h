#pragma once

namespace SCION_CORE::ECS
{
class Registry;
}

namespace SCION_CORE::Systems
{
class PhysicsSystem
{
  public:
	PhysicsSystem();
	~PhysicsSystem() = default;

	void Update( SCION_CORE::ECS::Registry& registry );
};
} // namespace SCION_CORE::Systems
