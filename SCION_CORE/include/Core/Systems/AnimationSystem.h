#pragma once

namespace SCION_CORE::ECS
{
class Registry;
}

namespace SCION_RENDERING
{
class Camera2D;
}

namespace SCION_CORE::Systems
{
class AnimationSystem
{
  public:
	AnimationSystem() = default;
	~AnimationSystem() = default;

	void Update( SCION_CORE::ECS::Registry& registry, SCION_RENDERING::Camera2D& camera );
};
} // namespace SCION_CORE::Systems
