#include "GridSystem.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Rendering/Core/RectBatchRenderer.h"
#include "Rendering/Core/Camera2D.h"
#include "Rendering/Essentials/Primitives.h"
#include "Rendering/Essentials/Vertex.h"
#include <Rendering/Essentials/Shader.h>
#include <Rendering/Essentials/Texture.h>

#include "editor/scene/SceneObject.h"

using namespace SCION_CORE;

namespace SCION_EDITOR
{
GridSystem::GridSystem()
	: m_pBatchRenderer{ std::make_unique<SCION_RENDERING::RectBatchRenderer>() }
{
}

void GridSystem::Update( SCION_CORE::Scene& currentScene, SCION_RENDERING::Camera2D& camera )
{
	if ( currentScene.GetMapType() == EMapType::IsoGrid )
	{
		UpdateIso( currentScene, camera );
		return;
	}

	auto& assetManager = MAIN_REGISTRY().GetAssetManager();
	const auto& canvas = currentScene.GetCanvas();
	auto camMat = camera.GetCameraMatrix();

	auto pColorShader = assetManager.GetShader( "color" );

	pColorShader->Enable();
	pColorShader->SetUniformMat4( "uProjection", camMat );

	m_pBatchRenderer->Begin();

	int tileWidth{ canvas.tileWidth }, tileHeight{ canvas.tileHeight };
	int canvasWidth{ canvas.width }, canvasHeight{ canvas.height };

	int cols = canvasWidth / tileWidth;
	int rows = canvasHeight / tileHeight;

	SCION_RENDERING::Color color{};

	for ( int row = 0; row < rows; row++ )
	{
		for ( int col = 0; col < cols; col++ )
		{
			// Create the checkboard colors
			if ( ( col - row ) % 2 == 0 && ( row - col ) % 2 == 0 )
				color = { 125, 125, 125, 70 };
			else
				color = { 200, 200, 200, 70 };

			SCION_RENDERING::Rect rect{
				.position = glm::vec2{ static_cast<float>( col * tileWidth ), static_cast<float>( row * tileHeight ) },
				.width = static_cast<float>( tileWidth ),
				.height = static_cast<float>( tileHeight ),
				.color = color };

			m_pBatchRenderer->AddRect( rect );
		}
	}

	m_pBatchRenderer->End();
	m_pBatchRenderer->Render();

	pColorShader->Disable();
}

void GridSystem::UpdateIso( SCION_CORE::Scene& currentScene, SCION_RENDERING::Camera2D& camera )
{
	auto& assetManager = MAIN_REGISTRY().GetAssetManager();
	const auto& canvas = currentScene.GetCanvas();
	auto camMat = camera.GetCameraMatrix();

	auto pColorShader = assetManager.GetShader( "color" );

	pColorShader->Enable();
	pColorShader->SetUniformMat4( "uProjection", camMat );

	m_pBatchRenderer->Begin();

	// Hard-coded, forcing tilewidth to be 2x canvas tile width.
	// TODO: This needs to be adjusted to automatically change the width/height when
	// adjusting settings in iso mode.

	int tileWidth{ canvas.tileWidth * 2 };
	int tileHalfWidth{ tileWidth / 2 };
	int tileHeight{ canvas.tileHeight };
	int tileHalfHeight{ canvas.tileHeight / 2 };
	int canvasWidth{ canvas.width };
	int canvasHeight{ canvas.height };

	int cols = canvasWidth / tileWidth;
	int rows = canvasHeight / tileHeight;

	SCION_RENDERING::Color color{};

	for ( int row = 0; row < rows; row++ )
	{
		for ( int col = 0; col < cols; col++ )
		{
			// Create the checkboard colors
			if ( ( col - row ) % 2 == 0 && ( row - col ) % 2 == 0 )
				color = { 125, 125, 125, 70 };
			else
				color = { 200, 200, 200, 70 };

			// Currently we are not going to use the canvas offset. We have control of the camera, so going into the negatives
			// should not really matter.
			SCION_RENDERING::Rect rect{ .position =
											glm::vec2{ static_cast<float>( ( /*canvas.offset.x +*/ tileHalfWidth ) +
																		   ( row - col ) * tileHalfWidth ),
													   static_cast<float>( ( row + col ) * tileHalfHeight ) },
										.width = static_cast<float>( tileWidth ),
										.height = static_cast<float>( tileHeight ),
										.color = color };

			m_pBatchRenderer->AddIsoRect( rect );
		}
	}

	m_pBatchRenderer->End();
	m_pBatchRenderer->Render();

	pColorShader->Disable();
}

} // namespace SCION_EDITOR
