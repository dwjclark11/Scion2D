#include "AnimationComponent.h"
#include <entt.hpp>

void SCION_CORE::ECS::AnimationComponent::CreateAnimationLuaBind(sol::state& lua)
{
	lua.new_usertype<AnimationComponent>(
		"Animation",
		"type_id", &entt::type_hash<AnimationComponent>::value,
		sol::call_constructor,
		sol::factories([](int numFrames, int frameRate, int frameOffset, bool bVertical) {
			return AnimationComponent{
					.numFrames = numFrames,
					.frameRate = frameRate,
					.frameOffset = frameOffset,
					.bVertical = bVertical
			};
			}
		),
		"num_frames", &AnimationComponent::numFrames,
		"frame_rate", &AnimationComponent::frameRate,
		"frame_offset", &AnimationComponent::frameOffset,
		"current_frame", &AnimationComponent::currentFrame,
		"bVertical", &AnimationComponent::bVertical
	);
}
