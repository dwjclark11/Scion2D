#include "CreateTileTool.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/MainRegistry.h"
#include "Rendering/Core/Camera2D.h"
#include "Logger/Logger.h"
#include "editor/utilities/EditorUtilities.h"

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
		// TODO: add collider component
	}

	if ( m_pMouseTile->bAnimation )
	{
		// TODO: add animation component
	}

	if ( m_pMouseTile->bPhysics )
	{
		// TODO: add physics component
	}

	tile.AddComponent<TileComponent>( static_cast<uint32_t>( tile.GetEntity() ) );
}

void CreateTileTool::RemoveTile()
{
	const auto& mouseWorldCoords = GetMouseWorldCoords();

	// Check if there is a tile that we can remove
	if ( auto id = CheckForTile( mouseWorldCoords ); id != entt::null )
	{
		// Create an entity from that id
		Entity tileToRemove{ CreateEntity( id ) };

		// TODO: We need to eventually get all of the component information from
		// the tile entity so we can place it into the Undo/Redo Commands that will be
		// coming in the future. For now we will just delete the tile.
		tileToRemove.Kill();
	}
}

CreateTileTool::CreateTileTool()
	: TileTool()
{
}

void CreateTileTool::Create()
{
	if ( OutOfBounds() || !IsActivated() || !SpriteValid() || !m_pRegistry )
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
	if ( OutOfBounds() || !IsActivated() || !SpriteValid() || !m_pCamera )
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
