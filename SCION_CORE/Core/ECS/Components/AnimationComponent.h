#pragma once
#include <sol/sol.hpp>
#include <SDL.h>

namespace SCION_CORE::ECS {
	struct AnimationComponent
	{
		int numFrames{ 1 }, frameRate{ 1 }, frameOffset{ 0 }, currentFrame{ 0 }; 
		int startTime{ static_cast<int>(SDL_GetTicks()) };
		bool bVertical{ false }, bLooped{ false };

		[[nodiscard]] std::string to_string() const;

		static void CreateAnimationLuaBind(sol::state& lua);
	};
}