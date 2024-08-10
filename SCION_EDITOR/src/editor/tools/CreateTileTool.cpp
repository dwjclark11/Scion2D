#include "CreateTileTool.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/MainRegistry.h"
#include "Rendering/Core/Camera2D.h"
#include "Logger/Logger.h"
#include "editor/utilities/EditorUtilities.h"

namespace SCION_EDITOR
{
CreateTileTool::CreateTileTool()
	: TileTool()
{
}

void CreateTileTool::Create()
{
	// TODO: Check out of bounds, active, etc

	// TODO: Check for mouse left button pressed.

	// Create tile entity based of of the mouse tile
}

void CreateTileTool::Draw()
{
	if ( !SpriteValid() || !IsActivated() || OutOfBounds() || !m_pCamera  )
		return;

	auto pShader = MAIN_REGISTRY().GetAssetManager().GetShader( "basic" );
	if ( !pShader )
		return;

	pShader->Enable();
	auto camMat = m_pCamera->GetCameraMatrix();
	pShader->SetUniformMat4( "uProjection", camMat);
	DrawMouseSprite();
	pShader->Disable();
}
} // namespace SCION_EDITOR