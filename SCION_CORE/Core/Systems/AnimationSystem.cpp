#include "AnimationSystem.h"
#include "../ECS/Components/AnimationComponent.h"
#include "../ECS/Components/SpriteComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include <SDL.h>

using namespace SCION_CORE::ECS;

namespace SCION_CORE::Systems {

	AnimationSystem::AnimationSystem(SCION_CORE::ECS::Registry& registry)
		: m_Registry{registry}
	{

	}

	void AnimationSystem::Update()
	{
		auto view = m_Registry.GetRegistry().view<AnimationComponent, SpriteComponent, TransformComponent>();

		for (auto entity : view)
		{
			const auto& transform = view.get<TransformComponent>(entity);
			auto& sprite = view.get<SpriteComponent>(entity);
			auto& animation = view.get<AnimationComponent>(entity);

			if (animation.numFrames <= 0)
				continue;
			
			// if we are not looped and the current from == num frames, skip
			if (!animation.bLooped && animation.currentFrame >= animation.numFrames - 1)
				continue;

			// Get the current frame
			animation.currentFrame = ((SDL_GetTicks() -animation.startTime) * animation.frameRate / 1000) % animation.numFrames;

			if (animation.bVertical)
			{
				sprite.uvs.v = animation.currentFrame * sprite.uvs.uv_width;
				sprite.uvs.u = animation.frameOffset * sprite.uvs.uv_width;
			}
			else
			{
				sprite.uvs.u = (animation.currentFrame * sprite.uvs.uv_width) + (animation.frameOffset * sprite.uvs.uv_width);
			}
		}
	}
}