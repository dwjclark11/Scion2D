#include "Core/Systems/AnimationSystem.h"
#include "Core/ECS/Components/AnimationComponent.h"
#include "Core/ECS/Components/SpriteComponent.h"
#include "Core/ECS/Components/TransformComponent.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/ECS/Registry.h"

#include <SDL.h>

using namespace SCION_CORE::ECS;

namespace SCION_CORE::Systems
{

void AnimationSystem::Update( SCION_CORE::ECS::Registry& registry, SCION_RENDERING::Camera2D& camera )
{
	auto view = registry.GetRegistry().view<AnimationComponent, SpriteComponent, TransformComponent>();
	if ( view.size_hint() < 1 )
		return;

	for ( auto entity : view )
	{
		const auto& transform = view.get<TransformComponent>( entity );
		auto& sprite = view.get<SpriteComponent>( entity );
		auto& animation = view.get<AnimationComponent>( entity );

		if ( !SCION_CORE::EntityInView( transform, sprite.width, sprite.height, camera ) )
			continue;

		if ( animation.numFrames <= 0 )
			continue;

		// if we are not looped and the current from == num frames, skip
		if ( !animation.bLooped && animation.currentFrame >= animation.numFrames - 1 )
			continue;

		// Get the current frame
		animation.currentFrame =
			( ( SDL_GetTicks() - animation.startTime ) * animation.frameRate / 1000 ) % animation.numFrames;

		if ( animation.bVertical )
		{
			sprite.uvs.v = animation.currentFrame * sprite.uvs.uv_width;
			sprite.uvs.u = animation.frameOffset * sprite.uvs.uv_width;
		}
		else
		{
			sprite.uvs.u =
				( animation.currentFrame * sprite.uvs.uv_width ) + ( animation.frameOffset * sprite.uvs.uv_width );
		}
	}
}
} // namespace SCION_CORE::Systems
