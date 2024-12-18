#include "RectToolCmds.h"
#include "Core/ECS/Registry.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/Components/AllComponents.h"

#include "Logger/Logger.h"
#include "editor/utilities/EditorUtilities.h"

using namespace SCION_CORE::ECS;

namespace SCION_EDITOR
{
void RectToolAddTilesCmd::undo()
{
	SCION_ASSERT( pRegistry && "The registry cannot be nullptr." );

	if ( !pRegistry )
	{
		SCION_ERROR( "Failed to undo create tile. Registry was not set correctly." );
		return;
	}

	auto tileView = pRegistry->GetRegistry().view<TileComponent, TransformComponent>();
	for ( const auto& tile : tiles )
	{
		const auto& tilePos = tile.transform.position;
		const auto layer = tile.sprite.layer;

		for ( auto entity : tileView )
		{
			Entity checkedTile{ *pRegistry, entity };
			const auto& transform = checkedTile.GetComponent<TransformComponent>();
			const auto& sprite = checkedTile.GetComponent<SpriteComponent>();

			if ( tilePos.x >= transform.position.x &&
				 tilePos.x < transform.position.x + sprite.width * transform.scale.x &&
				 tilePos.y >= transform.position.y &&
				 tilePos.y < transform.position.y + sprite.height * transform.scale.y && layer == sprite.layer )
			{
				if ( entity != entt::null )
				{
					pRegistry->GetRegistry().destroy( entity );
				}

				break;
			}
		}
	}
}

void RectToolAddTilesCmd::redo()
{
	SCION_ASSERT( pRegistry && "The registry cannot be nullptr." );

	if ( !pRegistry )
	{
		SCION_ERROR( "Failed to undo create tile. Registry was not set correctly." );
		return;
	}

	for ( const auto& tile : tiles )
	{
		Entity addedTile{ *pRegistry, "", "" };

		addedTile.AddComponent<TransformComponent>( tile.transform );
		addedTile.AddComponent<SpriteComponent>( tile.sprite );
		addedTile.AddComponent<TileComponent>( static_cast<std::uint32_t>( addedTile.GetEntity() ) );

		if ( tile.bAnimation )
		{
			addedTile.AddComponent<AnimationComponent>( tile.animation );
		}

		if ( tile.bCollider )
		{
			addedTile.AddComponent<BoxColliderComponent>( tile.boxCollider );
		}

		if ( tile.bCircle )
		{
			addedTile.AddComponent<CircleColliderComponent>( tile.circleCollider );
		}

		if ( tile.bPhysics )
		{
			addedTile.AddComponent<PhysicsComponent>( tile.physics );
		}
	}
}

void RectToolRemoveTilesCmd::undo()
{
	SCION_ASSERT( pRegistry && "The registry cannot be nullptr." );

	if ( !pRegistry )
	{
		SCION_ERROR( "Failed to undo create tile. Registry was not set correctly." );
		return;
	}

	for ( const auto& tile : tiles )
	{
		Entity addedTile{ *pRegistry, "", "" };

		addedTile.AddComponent<TransformComponent>( tile.transform );
		addedTile.AddComponent<SpriteComponent>( tile.sprite );
		addedTile.AddComponent<TileComponent>( static_cast<std::uint32_t>( addedTile.GetEntity() ) );

		if ( tile.bAnimation )
		{
			addedTile.AddComponent<AnimationComponent>( tile.animation );
		}

		if ( tile.bCollider )
		{
			addedTile.AddComponent<BoxColliderComponent>( tile.boxCollider );
		}

		if ( tile.bCircle )
		{
			addedTile.AddComponent<CircleColliderComponent>( tile.circleCollider );
		}

		if ( tile.bPhysics )
		{
			addedTile.AddComponent<PhysicsComponent>( tile.physics );
		}
	}
}

void RectToolRemoveTilesCmd::redo()
{
	SCION_ASSERT( pRegistry && "The registry cannot be nullptr." );

	if ( !pRegistry )
	{
		SCION_ERROR( "Failed to undo create tile. Registry was not set correctly." );
		return;
	}

	auto tileView = pRegistry->GetRegistry().view<TileComponent, TransformComponent>();
	for ( const auto& tile : tiles )
	{
		const auto& tilePos = tile.transform.position;
		const auto layer = tile.sprite.layer;

		for ( auto entity : tileView )
		{
			Entity checkedTile{ *pRegistry, entity };
			const auto& transform = checkedTile.GetComponent<TransformComponent>();
			const auto& sprite = checkedTile.GetComponent<SpriteComponent>();

			if ( tilePos.x >= transform.position.x &&
				 tilePos.x < transform.position.x + sprite.width * transform.scale.x &&
				 tilePos.y >= transform.position.y &&
				 tilePos.y < transform.position.y + sprite.height * transform.scale.y && layer == sprite.layer )
			{
				if ( entity != entt::null )
				{
					pRegistry->GetRegistry().destroy( entity );
				}

				break;
			}
		}
	}
}

} // namespace SCION_EDITOR
