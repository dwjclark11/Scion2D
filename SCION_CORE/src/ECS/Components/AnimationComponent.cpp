#include "Core/ECS/Components/AnimationComponent.h"
#include <entt/entt.hpp>

std::string Scion::Core::ECS::AnimationComponent::to_string() const
{
	// Current frame does not need to be returned because it is a calculated value
	std::stringstream ss;
	ss << "==== Animation Component ==== \n"
	   << std::boolalpha << "Num Frames: " << numFrames << "\n"
	   << "Frame Rate: " << frameRate << "\n"
	   << "bVertical: " << bVertical << "\n"
	   << "bLooped: " << bLooped << "\n";

	return ss.str();
}

void Scion::Core::ECS::AnimationComponent::CreateAnimationLuaBind( sol::state& lua )
{
	lua.new_usertype<AnimationComponent>(
		"Animation",
		"type_id",
		&entt::type_hash<AnimationComponent>::value,
		sol::call_constructor,
		sol::factories( []( int numFrames, int frameRate, bool bVertical, bool bLooped ) {
			return AnimationComponent{ .numFrames = numFrames,
									   .frameRate = frameRate,
									   .bVertical = bVertical,
									   .bLooped = bLooped };
		} ),
		"numFrames",
		&AnimationComponent::numFrames,
		"frameRate",
		&AnimationComponent::frameRate,
		"currentFrame",
		&AnimationComponent::currentFrame,
		"startTime",
		&AnimationComponent::startTime,
		"bVertical",
		&AnimationComponent::bVertical,
		"bLooped",
		&AnimationComponent::bLooped,
		"reset",
		[]( AnimationComponent& anim ) {
			anim.currentFrame = 0;
			anim.startTime = SDL_GetTicks();
		},
		"toString",
		&AnimationComponent::to_string );
}
