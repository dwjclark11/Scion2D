#include "editor/commands/cmds/TileCmds.h"
#include "editor/scene/SceneObject.h"
#include "Core/ECS/Registry.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/Components/AllComponents.h"

#include "Logger/Logger.h"
#include "editor/utilities/EditorUtilities.h"

using namespace Scion::Core;
using namespace Scion::Core::ECS;

namespace Scion::Editor
{

void RemoveTileLayerCmd::undo()
{
	SCION_ASSERT( pSceneObject && "The SceneObject cannot be nullptr." );

	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to undo remove tile layer. SceneObject was not set correctly." );
		return;
	}

	auto* pRegistry = pSceneObject->GetRegistryPtr();
	if ( !pRegistry )
	{
		SCION_ERROR( "Failed to undo remove tile layer. Scene Registry was invalid." );
		return;
	}

	// Push each layer above this up one layer
	for ( auto& layerParam : pSceneObject->GetLayerParams() )
	{
		if ( layerParam.layer >= spriteLayerParams.layer )
		{
			layerParam.layer++;
		}
	}

	pSceneObject->AddLayer( spriteLayerParams );

	// Push each sprite up one layer
	auto tileView = pRegistry->GetRegistry().view<TileComponent, SpriteComponent>();
	for ( auto entity : tileView )
	{
		Entity ent{ pRegistry, entity };
		if ( auto* pSprite = ent.TryGetComponent<SpriteComponent>() )
		{
			if ( pSprite->layer >= spriteLayerParams.layer )
			{
				pSprite->layer++;
			}
		}
	}

	// Add the tiles back into the registry
	for ( const auto& tile : tilesRemoved )
	{
		Entity ent{ pRegistry, "", "" };
		ent.AddComponent<TransformComponent>( tile.transform );
		ent.AddComponent<SpriteComponent>( tile.sprite );
		ent.AddComponent<TileComponent>( static_cast<std::uint32_t>( ent.GetEntity() ) );

		if ( tile.bCollider )
		{
			ent.AddComponent<BoxColliderComponent>( tile.boxCollider );
		}

		if ( tile.bPhysics )
		{
			ent.AddComponent<PhysicsComponent>( tile.physics );
		}

		if ( tile.bAnimation )
		{
			ent.AddComponent<AnimationComponent>( tile.animation );
		}

		if ( tile.bCircle )
		{
			ent.AddComponent<CircleColliderComponent>( tile.circleCollider );
		}
	}
}

void RemoveTileLayerCmd::redo()
{
	SCION_ASSERT( pSceneObject && "The SceneObject cannot be nullptr." );

	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to undo remove tile layer. SceneObject was not set correctly." );
		return;
	}

	auto* pRegistry = pSceneObject->GetRegistryPtr();
	if ( !pRegistry )
	{
		SCION_ERROR( "Failed to undo remove tile layer. Scene Registry was invalid." );
		return;
	}

	auto& layerParams = pSceneObject->GetLayerParams();

	layerParams.erase( layerParams.begin() + spriteLayerParams.layer );

	// Drop all layers above removed layer down by 1
	for ( auto& spriteLayer : layerParams )
	{
		if ( spriteLayer.layer > spriteLayerParams.layer )
		{
			spriteLayer.layer--;
		}
	}

	auto view = pRegistry->GetRegistry().view<TileComponent, SpriteComponent>();
	for ( auto entity : view )
	{
		Entity ent{ pRegistry, entity };
		auto& sprite = ent.GetComponent<SpriteComponent>();
		if ( sprite.layer == spriteLayerParams.layer )
		{
			ent.Destroy();
		}
		else if ( sprite.layer > spriteLayerParams.layer ) // Drop the layer down if greater.
		{
			sprite.layer--;
		}
	}
}

void AddTileLayerCmd::undo()
{
	SCION_ASSERT( pSceneObject && "The SceneObject cannot be nullptr." );

	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to undo add tile layer. SceneObject was not set correctly." );
		return;
	}

	auto& layerParams = pSceneObject->GetLayerParams();
	layerParams.erase( layerParams.begin() + spriteLayerParams.layer );
}

void AddTileLayerCmd::redo()
{
	SCION_ASSERT( pSceneObject && "The SceneObject cannot be nullptr." );

	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to redo add tile layer. SceneObject was not set correctly." );
		return;
	}

	auto& layerParams = pSceneObject->GetLayerParams();
	layerParams.push_back( spriteLayerParams );
}

// struct MoveTileLayerCmd
//{
//	SceneObject* pSceneObject{ nullptr };
//	int from;
//	int to;

void MoveTileLayerCmd::undo()
{
	SCION_ASSERT( pSceneObject && "The SceneObject cannot be nullptr." );

	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to redo add tile layer. SceneObject was not set correctly." );
		return;
	}

	auto* pRegistry = pSceneObject->GetRegistryPtr();
	SCION_ASSERT( pRegistry && "Registry must be valid." );

	// Change the layers
	auto& layerParams = pSceneObject->GetLayerParams();
	const int nextLayer = layerParams[ to ].layer;
	layerParams[ to ].layer = layerParams[ from ].layer;
	layerParams[ from ].layer = nextLayer;
	std::swap( layerParams[ from ], layerParams[ to ] );

	auto tileView = pRegistry->GetRegistry().view<TileComponent, SpriteComponent>();
	for ( auto entity : tileView )
	{
		Entity ent{ pRegistry, entity };
		if ( auto* sprite = ent.TryGetComponent<SpriteComponent>() )
		{
			if ( sprite->layer == to )
			{
				sprite->layer = from;
			}
			else if ( sprite->layer == from )
			{
				sprite->layer = to;
			}
		}
	}
}

void MoveTileLayerCmd::redo()
{
	SCION_ASSERT( pSceneObject && "The SceneObject cannot be nullptr." );

	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to redo add tile layer. SceneObject was not set correctly." );
		return;
	}

	auto* pRegistry = pSceneObject->GetRegistryPtr();
	SCION_ASSERT( pRegistry && "Registry must be valid." );

	// Change the layers
	auto& layerParams = pSceneObject->GetLayerParams();
	const int nextLayer = layerParams[ from ].layer;
	layerParams[ from ].layer = layerParams[ to ].layer;
	layerParams[ to ].layer = nextLayer;
	std::swap( layerParams[ from ], layerParams[ to ] );

	auto tileView = pRegistry->GetRegistry().view<TileComponent, SpriteComponent>();
	for ( auto entity : tileView )
	{
		Entity ent{ pRegistry, entity };
		if ( auto* sprite = ent.TryGetComponent<SpriteComponent>() )
		{
			if ( sprite->layer == to )
			{
				sprite->layer = from;
			}
			else if ( sprite->layer == from )
			{
				sprite->layer = to;
			}
		}
	}
}

void ChangeTileLayerNameCmd::undo()
{
	SCION_ASSERT( pSceneObject && "The SceneObject cannot be nullptr." );

	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to undo change tile layer name. SceneObject was not set correctly." );
		return;
	}

	auto& layerParams = pSceneObject->GetLayerParams();
	auto layerItr = std::ranges::find( layerParams, sNewName, &Scion::Utilities::SpriteLayerParams::sLayerName );
	SCION_ASSERT( layerItr != layerParams.end() && "Failed to find layer." );
	layerItr->sLayerName = sOldName;
}

void ChangeTileLayerNameCmd::redo()
{
	SCION_ASSERT( pSceneObject && "The SceneObject cannot be nullptr." );

	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to redo change tile layer name. SceneObject was not set correctly." );
		return;
	}

	auto& layerParams = pSceneObject->GetLayerParams();
	auto layerItr = std::ranges::find( layerParams, sOldName, &Scion::Utilities::SpriteLayerParams::sLayerName );
	SCION_ASSERT( layerItr != layerParams.end() && "Failed to find layer." );
	layerItr->sLayerName = sNewName;
}

} // namespace Scion::Editor
