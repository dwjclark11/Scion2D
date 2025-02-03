#pragma once
#include <sol/sol.hpp>

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

	static void CreateAnimationSystemLuaBind( sol::state& lua, SCION_CORE::ECS::Registry& registry );
};
} // namespace SCION_CORE::Systems
