#pragma once
#include <sol/sol.hpp>

namespace Scion::Core::ECS
{
class Registry;
}

namespace Scion::Rendering
{
class Camera2D;
}

namespace Scion::Core::Systems
{
class AnimationSystem
{
  public:
	AnimationSystem() = default;
	~AnimationSystem() = default;

	void Update( Scion::Core::ECS::Registry& registry, Scion::Rendering::Camera2D& camera );

	static void CreateAnimationSystemLuaBind( sol::state& lua, Scion::Core::ECS::Registry& registry );
};
} // namespace Scion::Core::Systems
