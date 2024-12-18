#include "GridSystem.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Rendering/Core/RectBatchRenderer.h"
#include "Rendering/Core/Camera2D.h"
#include "Rendering/Essentials/Primitives.h"
#include "Rendering/Essentials/Vertex.h"
#include "editor/scene/SceneObject.h"

namespace SCION_EDITOR
{
GridSystem::GridSystem()
	: m_pBatchRenderer{ std::make_unique<SCION_RENDERING::RectBatchRenderer>() }
{
}

void GridSystem::Update( SceneObject& currentScene, SCION_RENDERING::Camera2D& camera )
{
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
} // namespace SCION_EDITOR
