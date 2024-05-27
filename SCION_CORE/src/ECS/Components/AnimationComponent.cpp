#include "Core/ECS/Components/AnimationComponent.h"
#include <entt/entt.hpp>

std::string SCION_CORE::ECS::AnimationComponent::to_string() const
{
	// Current frame does not need to be returned because it is a calculated value
	std::stringstream ss;
	ss <<
		"==== Animation Component ==== \n" << std::boolalpha <<
		"Num Frames: " << numFrames << "\n" <<
		"Frame Rate: " << frameRate<< "\n" <<
		"Frame Offset: " << frameOffset << "\n" <<
		"bVertical: " << bVertical << "\n" <<
		"bLooped: " << bLooped << "\n";

	return ss.str();
}

void SCION_CORE::ECS::AnimationComponent::CreateAnimationLuaBind(sol::state& lua)
{
	lua.new_usertype<AnimationComponent>(
		"Animation",
		"type_id", &entt::type_hash<AnimationComponent>::value,
		sol::call_constructor,
		sol::factories([](int numFrames, int frameRate, int frameOffset, bool bVertical, bool bLooped) {
			return AnimationComponent{
					.numFrames = numFrames,
					.frameRate = frameRate,
					.frameOffset = frameOffset,
					.bVertical = bVertical,
					.bLooped = bLooped
			};
			}
		),
		"num_frames", &AnimationComponent::numFrames,
		"frame_rate", &AnimationComponent::frameRate,
		"frame_offset", &AnimationComponent::frameOffset,
		"current_frame", &AnimationComponent::currentFrame,
		"start_time", &AnimationComponent::startTime,
		"bVertical", &AnimationComponent::bVertical,
		"bLooped", &AnimationComponent::bLooped,
		"reset", [](AnimationComponent& anim) { 
			anim.currentFrame = 0;
			anim.startTime = SDL_GetTicks(); 
		},
		"to_string", &AnimationComponent::to_string
	);
}
