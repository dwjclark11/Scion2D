#include "CreateTileTool.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/MainRegistry.h"
#include "Rendering/Core/Camera2D.h"
#include "Logger/Logger.h"

#include "editor/utilities/EditorUtilities.h"
#include "editor/commands/CommandManager.h"

#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"

using namespace SCION_CORE::ECS;

namespace SCION_EDITOR
{
void CreateTileTool::CreateNewTile()
{
	const auto& mouseWorldCoords = GetMouseWorldCoords();

	// Check if there is already a tile
	if ( auto id = CheckForTile( mouseWorldCoords ); id != entt::null )
		return;

	Entity tile{ CreateEntity() };

	tile.AddComponent<TransformComponent>( m_pMouseTile->transform );
	tile.AddComponent<SpriteComponent>( m_pMouseTile->sprite );

	if ( m_pMouseTile->bCollider )
	{
		tile.AddComponent<BoxColliderComponent>( m_pMouseTile->boxCollider );
	}

	if ( m_pMouseTile->bCircle )
	{
		tile.AddComponent<CircleColliderComponent>( m_pMouseTile->circleCollider );
	}

	if ( m_pMouseTile->bAnimation )
	{
		tile.AddComponent<AnimationComponent>( m_pMouseTile->animation );
	}

	if ( m_pMouseTile->bPhysics )
	{
		tile.AddComponent<PhysicsComponent>( m_pMouseTile->physics );
	}

	tile.AddComponent<TileComponent>( static_cast<uint32_t>( tile.GetEntity() ) );

	auto createToolAddCmd =
		UndoableCommands{ CreateTileToolAddCmd{ .pRegistry = SCENE_MANAGER().GetCurrentScene()->GetRegistryPtr(),
												.pTile = std::make_shared<Tile>( *m_pMouseTile ) } };

	COMMAND_MANAGER().Execute( createToolAddCmd );
}

void CreateTileTool::RemoveTile()
{
	const auto& mouseWorldCoords = GetMouseWorldCoords();

	// Check if there is a tile that we can remove
	if ( auto id = CheckForTile( mouseWorldCoords ); id != entt::null )
	{
		// Create an entity from that id
		Entity tileToRemove{ CreateEntity( id ) };
		Tile removedTile{};

		removedTile.transform = tileToRemove.GetComponent<TransformComponent>();
		removedTile.sprite = tileToRemove.GetComponent<SpriteComponent>();

		if ( tileToRemove.HasComponent<BoxColliderComponent>() )
		{
			removedTile.bCollider = true;
			removedTile.boxCollider = tileToRemove.GetComponent<BoxColliderComponent>();
		}

		if ( tileToRemove.HasComponent<CircleColliderComponent>() )
		{
			removedTile.bCircle = true;
			removedTile.circleCollider = tileToRemove.GetComponent<CircleColliderComponent>();
		}

		if ( tileToRemove.HasComponent<AnimationComponent>() )
		{
			removedTile.bAnimation = true;
			removedTile.animation = tileToRemove.GetComponent<AnimationComponent>();
		}

		if ( tileToRemove.HasComponent<PhysicsComponent>() )
		{
			removedTile.bPhysics = true;
			removedTile.physics = tileToRemove.GetComponent<PhysicsComponent>();
		}

		tileToRemove.Kill();

		auto createToolRemoveCmd =
			UndoableCommands{ CreateTileToolRemoveCmd{ .pRegistry = SCENE_MANAGER().GetCurrentScene()->GetRegistryPtr(),
													   .pTile = std::make_shared<Tile>( removedTile ) } };

		COMMAND_MANAGER().Execute( createToolRemoveCmd );
	}
}

CreateTileTool::CreateTileTool()
	: TileTool()
{
}

void CreateTileTool::Create()
{
	if ( !CanDrawOrCreate() )
		return;

	if ( MouseBtnJustPressed( EMouseButton::LEFT ) || MouseBtnPressed( EMouseButton::LEFT ) )
	{
		CreateNewTile();
	}
	else if ( MouseBtnJustPressed( EMouseButton::RIGHT ) || MouseBtnPressed( EMouseButton::RIGHT ) )
	{
		RemoveTile();
	}
}

void CreateTileTool::Draw()
{
	if ( !CanDrawOrCreate() )
		return;

	auto pShader = MAIN_REGISTRY().GetAssetManager().GetShader( "basic" );
	if ( !pShader )
		return;

	pShader->Enable();
	auto camMat = m_pCamera->GetCameraMatrix();
	pShader->SetUniformMat4( "uProjection", camMat );
	DrawMouseSprite();
	pShader->Disable();
}
} // namespace SCION_EDITOR
