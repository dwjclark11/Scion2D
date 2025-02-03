#include "RectFillTool.h"
#include "Rendering/Core/RectBatchRenderer.h"
#include "Rendering/Core/BatchRenderer.h"
#include "Rendering/Core/Camera2D.h"
#include "Rendering/Essentials/Primitives.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Logger/Logger.h"

#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/commands/CommandManager.h"

using namespace SCION_RENDERING;
using namespace SCION_CORE::ECS;

constexpr int PREVIEW_LAYER = 10;

namespace SCION_EDITOR
{
void RectFillTool::CreateTiles()
{
	const auto& sprite = m_pMouseTile->sprite;
	const auto& transform = m_pMouseTile->transform;
	auto pTexture = MAIN_REGISTRY().GetAssetManager().GetTexture( sprite.sTextureName );
	if ( !pTexture )
		return;

	const int dx = static_cast<int>( m_pTileFillRect->width );
	const int dy = static_cast<int>( m_pTileFillRect->height );

	auto spriteWidth = static_cast<int>( sprite.width * transform.scale.x * ( dx > 0 ? 1.f : -1.f ) );
	auto spriteHeight = static_cast<int>( sprite.height * transform.scale.y * ( dy > 0 ? 1.f : -1.f ) );

	std::vector<Tile> createdTiles;

	for ( int y = 0; ( dy > 0 ? y < dy : y > dy ); y += spriteHeight )
	{
		for ( int x = 0; ( dx > 0 ? x < dx : x > dx ); x += spriteWidth )
		{
			glm::vec2 newTilePosition{ m_StartPressPos.x + x, m_StartPressPos.y + y };

			// Check if there is already a tile
			if ( auto id = CheckForTile( newTilePosition ); id != entt::null )
				continue;

			Entity tile{ CreateEntity() };
			Tile createdTile{};

			auto& newTransform = tile.AddComponent<TransformComponent>( transform );
			newTransform.position = newTilePosition;
			createdTile.transform = newTransform;

			tile.AddComponent<SpriteComponent>( sprite );

			createdTile.sprite = sprite;

			if ( m_pMouseTile->bCollider )
			{
				tile.AddComponent<BoxColliderComponent>( m_pMouseTile->boxCollider );
				createdTile.boxCollider = m_pMouseTile->boxCollider;
				createdTile.bCollider = true;
			}

			if ( m_pMouseTile->bCircle )
			{
				tile.AddComponent<CircleColliderComponent>( m_pMouseTile->circleCollider );
				createdTile.circleCollider = m_pMouseTile->circleCollider;
				createdTile.bCircle = true;
			}

			if ( m_pMouseTile->bAnimation )
			{
				tile.AddComponent<AnimationComponent>( m_pMouseTile->animation );
				createdTile.animation = m_pMouseTile->animation;
				createdTile.bAnimation = true;
			}

			if ( m_pMouseTile->bPhysics )
			{
				tile.AddComponent<PhysicsComponent>( m_pMouseTile->physics );
				createdTile.physics = m_pMouseTile->physics;
				createdTile.bPhysics = true;
			}

			tile.AddComponent<TileComponent>( static_cast<uint32_t>( tile.GetEntity() ) );
			createdTiles.push_back( createdTile );
		}
	}

	auto rectToolAddCmd = UndoableCommands{ RectToolAddTilesCmd{
		.pRegistry = SCENE_MANAGER().GetCurrentScene()->GetRegistryPtr(), .tiles = createdTiles } };

	COMMAND_MANAGER().Execute( rectToolAddCmd );
}

void RectFillTool::RemoveTiles()
{
	const auto& sprite = m_pMouseTile->sprite;
	const auto& transform = m_pMouseTile->transform;
	const int dx = static_cast<int>( m_pTileFillRect->width );
	const int dy = static_cast<int>( m_pTileFillRect->height );

	auto spriteWidth = static_cast<int>( sprite.width * transform.scale.x * ( dx > 0 ? 1.f : -1.f ) );
	auto spriteHeight = static_cast<int>( sprite.height * transform.scale.y * ( dy > 0 ? 1.f : -1.f ) );

	std::set<std::uint32_t> entitiesToRemove{};

	for ( int y = 0; ( dy > 0 ? y < dy : y > dy ); y += spriteHeight )
	{
		for ( int x = 0; ( dx > 0 ? x < dx : x > dx ); x += spriteWidth )
		{
			if ( auto id = CheckForTile( glm::vec2{ m_StartPressPos.x + x, m_StartPressPos.y + y } ); id != entt::null )
			{
				entitiesToRemove.insert( id );
			}
		}
	}

	std::vector<Tile> removedTiles{};

	for ( auto id : entitiesToRemove )
	{
		Entity tileToRemove{ CreateEntity( id ) };
		Tile removedTile{};

		removedTile.transform = tileToRemove.GetComponent<TransformComponent>();
		removedTile.sprite = tileToRemove.GetComponent<SpriteComponent>();

		if ( tileToRemove.HasComponent<BoxColliderComponent>() )
		{
			removedTile.boxCollider = tileToRemove.GetComponent<BoxColliderComponent>();
			removedTile.bCollider = true;
		}

		if ( tileToRemove.HasComponent<CircleColliderComponent>() )
		{
			removedTile.circleCollider = tileToRemove.GetComponent<CircleColliderComponent>();
			removedTile.bCircle = true;
		}

		if ( tileToRemove.HasComponent<AnimationComponent>() )
		{
			removedTile.animation = tileToRemove.GetComponent<AnimationComponent>();
			removedTile.bAnimation = true;
		}

		if ( tileToRemove.HasComponent<PhysicsComponent>() )
		{
			removedTile.physics = tileToRemove.GetComponent<PhysicsComponent>();
			removedTile.bPhysics = true;
		}

		tileToRemove.Kill();
		removedTiles.push_back( removedTile );
	}

	auto rectToolRemovedCmd = UndoableCommands{ RectToolRemoveTilesCmd{
		.pRegistry = SCENE_MANAGER().GetCurrentScene()->GetRegistryPtr(), .tiles = removedTiles } };

	COMMAND_MANAGER().Execute( rectToolRemovedCmd );
}

void RectFillTool::DrawPreview( int dx, int dy )
{
	const auto& sprite = m_pMouseTile->sprite;
	const auto& transform = m_pMouseTile->transform;
	auto pTexture = MAIN_REGISTRY().GetAssetManager().GetTexture( sprite.sTextureName );
	if ( !pTexture )
		return;

	auto spriteWidth = static_cast<int>( sprite.width * transform.scale.x * ( dx > 0 ? 1.f : -1.f ) );
	auto spriteHeight = static_cast<int>( sprite.height * transform.scale.y * ( dy > 0 ? 1.f : -1.f ) );

	for ( int y = 0; ( dy > 0 ? y < dy : y > dy ); y += spriteHeight )
	{
		for ( int x = 0; ( dx > 0 ? x < dx : x > dx ); x += spriteWidth )
		{
			glm::vec4 tilePosition{ m_StartPressPos.x + x,
									m_StartPressPos.y + y,
									sprite.width * transform.scale.x,
									sprite.height * transform.scale.y };

			glm::vec4 uvs{ sprite.uvs.u, sprite.uvs.v, sprite.uvs.uv_width, sprite.uvs.uv_height };

			m_pBatchRenderer->AddSprite(
				tilePosition, uvs, pTexture->GetID(), PREVIEW_LAYER, glm::mat4{ 1.f }, sprite.color );
		}
	}
}

void RectFillTool::ResetTile()
{
	*m_pTileFillRect = Rect{};
}

RectFillTool::RectFillTool()
	: TileTool()
	, m_pShapeRenderer{ std::make_unique<RectBatchRenderer>() }
	, m_pTileFillRect{ std::make_unique<Rect>() }
	, m_StartPressPos{ 0.f }
{
}

RectFillTool::~RectFillTool()
{
}

void RectFillTool::Create()
{
	if ( !CanDrawOrCreate() )
		return;

	if ( MouseBtnJustPressed( EMouseButton::LEFT ) || MouseBtnJustPressed( EMouseButton::RIGHT ) )
		m_StartPressPos = GetMouseWorldCoords();

	if ( MouseBtnJustReleased( EMouseButton::LEFT ) )
	{
		CreateTiles();
		ResetTile();
	}
	else if ( MouseBtnJustReleased( EMouseButton::RIGHT ) )
	{
		RemoveTiles();
		ResetTile();
	}
}
void RectFillTool::Draw()
{
	if ( !CanDrawOrCreate() )
		return;

	auto pShader = MAIN_REGISTRY().GetAssetManager().GetShader( "basic" );
	auto pColorShader = MAIN_REGISTRY().GetAssetManager().GetShader( "color" );
	if ( !pShader || !pColorShader )
		return;

	pShader->Enable();
	auto camMat = m_pCamera->GetCameraMatrix();
	pShader->SetUniformMat4( "uProjection", camMat );
	DrawMouseSprite();

	bool bLeftMousePressed{ MouseBtnPressed( EMouseButton::LEFT ) };
	bool bRightMousePressed{ MouseBtnPressed( EMouseButton::RIGHT ) };
	if ( !bLeftMousePressed && !bRightMousePressed )
		return;

	const auto& mouseWorldCoords = GetMouseWorldCoords();
	auto dx = mouseWorldCoords.x - m_StartPressPos.x;
	auto dy = mouseWorldCoords.y - m_StartPressPos.y;

	const auto& sprite = m_pMouseTile->sprite;
	const auto& transform = m_pMouseTile->transform;

	auto spriteWidth = sprite.width * transform.scale.x;
	auto spriteHeight = sprite.height * transform.scale.y;

	dx += dx > 0 ? spriteWidth : -spriteWidth;
	dy += dy > 0 ? spriteHeight : -spriteHeight;

	auto newPosX = m_StartPressPos.x + ( dx > 0 ? 0 : spriteWidth );
	auto newPosY = m_StartPressPos.y + ( dy > 0 ? 0 : spriteHeight );

	Color color{};

	if ( bLeftMousePressed )
	{
		color = { 0x7D, 0xF9, 0xFF, 100 };
		m_pBatchRenderer->Begin();
		DrawPreview( dx, dy );
		m_pBatchRenderer->End();
		m_pBatchRenderer->Render();
		pShader->Disable();
	}
	else if ( bRightMousePressed )
	{
		color = { 0xFF, 0x73, 0x77, 100 };
	}

	pColorShader->Enable();
	m_pShapeRenderer->Begin();
	m_pShapeRenderer->AddRect( Rect{ .position = { newPosX, newPosY }, .width = dx, .height = dy, .color = color } );
	m_pShapeRenderer->End();
	m_pShapeRenderer->Render();
	pColorShader->Disable();

	m_pTileFillRect->position = { newPosX, newPosY };
	m_pTileFillRect->width = dx;
	m_pTileFillRect->height = dy;
}
} // namespace SCION_EDITOR
