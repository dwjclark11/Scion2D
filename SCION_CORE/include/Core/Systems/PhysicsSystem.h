#pragma once

namespace Scion::Core::ECS
{
class Registry;
}

namespace Scion::Core::Systems
{
class PhysicsSystem
{
  public:
	PhysicsSystem();
	~PhysicsSystem() = default;

	void Update( Scion::Core::ECS::Registry& registry );
};
} // namespace Scion::Core::Systems
