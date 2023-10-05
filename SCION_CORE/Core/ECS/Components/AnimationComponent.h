#pragma once
#include <sol/sol.hpp>

namespace SCION_CORE::ECS {
	struct AnimationComponent
	{
		int numFrames{ 1 }, frameRate{ 1 }, frameOffset{ 0 }, currentFrame{ 0 };
		bool bVertical{ false };
		static void CreateAnimationLuaBind(sol::state& lua);
	};
}