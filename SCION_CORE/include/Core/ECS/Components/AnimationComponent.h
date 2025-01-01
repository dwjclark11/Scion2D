#pragma once
#include <sol/sol.hpp>
#include <SDL.h>

namespace SCION_CORE::ECS
{
struct AnimationComponent
{
	/* Number of frames in the animation. */
	int numFrames{ 1 };
	/* The speed that the animation will run at. Frames per second. */
	int frameRate{ 1 };
	/* The position offset on the sprite sheet to start the animation. */
	int frameOffset{ 0 };
	/* The current frame the animation is at. */
	int currentFrame{ 0 };
	/* Uses an offset of the ticks to run the frames. This is the start time. */
	int startTime{ static_cast<int>( SDL_GetTicks() ) };
	/* Does the animation scroll vertically? */
	bool bVertical{ false };
	/* Is the animation looped? */
	bool bLooped{ false };

	[[nodiscard]] std::string to_string() const;

	static void CreateAnimationLuaBind( sol::state& lua );
};
} // namespace SCION_CORE::ECS
