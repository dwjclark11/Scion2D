#include "CreateTileToolCmds.h"

#include "Core/ECS/Registry.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/Components/AllComponents.h"

#include "editor/utilities/EditorUtilities.h"
#include "Logger/Logger.h"

using namespace SCION_CORE::ECS;

namespace SCION_EDITOR
{
void CreateTileToolAddCmd::undo()
{
	SCION_ASSERT( pRegistry && "The registry cannot be nullptr." );

	if ( !pRegistry )
	{
		SCION_ERROR( "Failed to undo create tile. Registry was not set correctly." );
		return;
	}

	SCION_ASSERT( pTile && "The tile cannot be nullptr." );

	if ( !pTile )
	{
		SCION_ERROR( "Failed to undo create tile. Tile was not set correctly." );
		return;
	}

	auto tiles = pRegistry->GetRegistry().view<TileComponent, TransformComponent>();
	const auto& tilePos = pTile->transform.position;
	entt::entity entityToRemove{ entt::null };

	for ( auto entity : tiles )
	{
		Entity tile{ *pRegistry, entity };
		const auto& transform = tile.GetComponent<TransformComponent>();
		const auto& sprite = tile.GetComponent<SpriteComponent>();

		if ( tilePos.x >= transform.position.x && tilePos.x < transform.position.x + sprite.width * transform.scale.x &&
			 tilePos.y >= transform.position.y &&
			 tilePos.y < transform.position.y + sprite.height * transform.scale.y &&
			 pTile->sprite.layer == sprite.layer )
		{
			entityToRemove = entity;
			break;
		}
	}

	SCION_ASSERT( entityToRemove != entt::null && "Entity should not be null." );
	if ( entityToRemove != entt::null )
	{
		Entity ent{ *pRegistry, entityToRemove };
		ent.Kill();
	}
}

void CreateTileToolAddCmd::redo()
{
	SCION_ASSERT( pRegistry && "The registry cannot be nullptr." );

	if ( !pRegistry )
	{
		SCION_ERROR( "Failed to undo create tile. Registry was not set correctly." );
		return;
	}

	SCION_ASSERT( pTile && "The tile cannot be nullptr." );

	if ( !pTile )
	{
		SCION_ERROR( "Failed to undo create tile. Tile was not set correctly." );
		return;
	}

	Entity tile{ *pRegistry, "", "" };
	tile.AddComponent<TransformComponent>( pTile->transform );
	tile.AddComponent<SpriteComponent>( pTile->sprite );
	tile.AddComponent<TileComponent>( static_cast<std::uint32_t>( tile.GetEntity() ) );

	if ( pTile->bAnimation )
	{
		tile.AddComponent<AnimationComponent>( pTile->animation );
	}

	if ( pTile->bCollider )
	{
		tile.AddComponent<BoxColliderComponent>( pTile->boxCollider );
	}

	if ( pTile->bCircle )
	{
		tile.AddComponent<CircleColliderComponent>( pTile->circleCollider );
	}

	if ( pTile->bPhysics )
	{
		tile.AddComponent<PhysicsComponent>( pTile->physics );
	}
}

void CreateTileToolRemoveCmd::undo()
{
	SCION_ASSERT( pRegistry && "The registry cannot be nullptr." );

	if ( !pRegistry )
	{
		SCION_ERROR( "Failed to undo create tile. Registry was not set correctly." );
		return;
	}

	SCION_ASSERT( pTile && "The tile cannot be nullptr." );

	if ( !pTile )
	{
		SCION_ERROR( "Failed to undo create tile. Tile was not set correctly." );
		return;
	}

	Entity tile{ *pRegistry, "", "" };
	tile.AddComponent<TransformComponent>( pTile->transform );
	tile.AddComponent<SpriteComponent>( pTile->sprite );
	tile.AddComponent<TileComponent>( static_cast<std::uint32_t>( tile.GetEntity() ) );

	if ( pTile->bAnimation )
	{
		tile.AddComponent<AnimationComponent>( pTile->animation );
	}

	if ( pTile->bCollider )
	{
		tile.AddComponent<BoxColliderComponent>( pTile->boxCollider );
	}

	if ( pTile->bCircle )
	{
		tile.AddComponent<CircleColliderComponent>( pTile->circleCollider );
	}

	if ( pTile->bPhysics )
	{
		tile.AddComponent<PhysicsComponent>( pTile->physics );
	}
}

void CreateTileToolRemoveCmd::redo()
{
	SCION_ASSERT( pRegistry && "The registry cannot be nullptr." );

	if ( !pRegistry )
	{
		SCION_ERROR( "Failed to undo create tile. Registry was not set correctly." );
		return;
	}

	SCION_ASSERT( pTile && "The tile cannot be nullptr." );

	if ( !pTile )
	{
		SCION_ERROR( "Failed to undo create tile. Tile was not set correctly." );
		return;
	}

	auto tiles = pRegistry->GetRegistry().view<TileComponent, TransformComponent>();
	const auto& tilePos = pTile->transform.position;
	entt::entity entityToRemove{ entt::null };

	for ( auto entity : tiles )
	{
		Entity tile{ *pRegistry, entity };
		const auto& transform = tile.GetComponent<TransformComponent>();
		const auto& sprite = tile.GetComponent<SpriteComponent>();

		if ( tilePos.x >= transform.position.x && tilePos.x < transform.position.x + sprite.width * transform.scale.x &&
			 tilePos.y >= transform.position.y &&
			 tilePos.y < transform.position.y + sprite.height * transform.scale.y &&
			 pTile->sprite.layer == sprite.layer )
		{
			entityToRemove = entity;
			break;
		}
	}

	SCION_ASSERT( entityToRemove != entt::null && "Entity should not be null." );
	if ( entityToRemove != entt::null )
	{
		Entity ent{ *pRegistry, entityToRemove };
		ent.Kill();
	}
}

} // namespace SCION_EDITOR
