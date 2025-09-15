#include "Core/Systems/AnimationSystem.h"
#include "Core/ECS/Components/AnimationComponent.h"
#include "Core/ECS/Components/SpriteComponent.h"
#include "Core/ECS/Components/TransformComponent.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/ECS/Registry.h"

#include "Logger/Logger.h"

#include <SDL.h>

using namespace SCION_CORE::ECS;
using namespace SCION_RENDERING;

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

		// We don't want to check if entities with UIComponents are out of the camera.
		// Since they use a different camera.
		if ( !registry.GetRegistry().all_of<UIComponent>( entity ) &&
			 !SCION_CORE::EntityInView( transform, sprite.width, sprite.height, camera ) )
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
			sprite.uvs.v = (animation.currentFrame + sprite.start_y) * sprite.uvs.uv_height;
			sprite.uvs.u = sprite.start_x * sprite.uvs.uv_width;
		}
		else
		{
			sprite.uvs.u = ( ( animation.currentFrame + sprite.start_x ) * sprite.uvs.uv_width );
			sprite.uvs.v = sprite.start_y * sprite.uvs.uv_height;
		}
	}
}

void AnimationSystem::CreateAnimationSystemLuaBind( sol::state& lua, SCION_CORE::ECS::Registry& registry )
{
	auto& pCamera = registry.GetContext<std::shared_ptr<Camera2D>>();

	SCION_ASSERT( pCamera && "A camera must exist in the current scene!" );

	lua.new_usertype<AnimationSystem>(
		"AnimationSystem",
		sol::call_constructor,
		sol::constructors<AnimationSystem()>(),
		"update",
		[ & ]( AnimationSystem& system, Registry& reg ) { system.Update( reg, *pCamera ); } );
}
} // namespace SCION_CORE::Systems
