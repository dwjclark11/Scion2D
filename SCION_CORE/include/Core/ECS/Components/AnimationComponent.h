#pragma once
#include <sol/sol.hpp>
#include <SDL.h>

namespace SCION_CORE::ECS
{
struct AnimationComponent
{
	int numFrames{ 1 };
	int frameRate{ 1 };
	int frameOffset{ 0 };
	int currentFrame{ 0 };
	int startTime{ static_cast<int>( SDL_GetTicks() ) };
	bool bVertical{ false };
	bool bLooped{ false };

	[[nodiscard]] std::string to_string() const;

	static void CreateAnimationLuaBind( sol::state& lua );
};
} // namespace SCION_CORE::ECS
