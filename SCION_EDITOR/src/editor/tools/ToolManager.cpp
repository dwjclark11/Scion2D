#include "ToolManager.h"
#include "CreateTileTool.h"
#include "RectFillTool.h"
#include "ToolAccessories.h"
#include "editor/scene/SceneObject.h"
#include "Rendering/Core/Camera2D.h"

namespace SCION_EDITOR
{
ToolManager::ToolManager()
{
	m_mapTools.emplace( EToolType::CREATE_TILE, std::make_unique<CreateTileTool>() );
	m_mapTools.emplace( EToolType::RECT_FILL_TILE, std::make_unique<RectFillTool>() );

	// TODO: Add other tools as needed.

	SetToolActive( EToolType::RECT_FILL_TILE );
}

void ToolManager::Update( Canvas& canvas )
{
	auto activeTool = std::ranges::find_if( m_mapTools, []( const auto& tool ) { return tool.second->IsActivated(); } );
	if ( activeTool != m_mapTools.end() )
		activeTool->second->Update( canvas );
}

void ToolManager::SetToolActive( EToolType eToolType )
{
	// TODO: Deactivate all gizmos when map exists
	m_eActiveGizmoType = EGizmoType::NO_GIZMO;

	for ( const auto& [ eType, pTool ] : m_mapTools )
	{
		if ( eType == eToolType )
			pTool->Activate();
		else
			pTool->Deactivate();
	}

	m_eActiveToolType = eToolType;
}

void ToolManager::SetGizmoActive( EGizmoType eGizmoType )
{
	// Deactivate all tools
	for ( const auto& [ eType, pTool ] : m_mapTools )
	{
		pTool->Deactivate();
	}
	m_eActiveToolType = EToolType::NO_TOOL;

	// Activate the specified gizmo
	// TODO: Create the gizmo map and set active

	m_eActiveGizmoType = eGizmoType;
}

TileTool* ToolManager::GetActiveTool()
{
	auto activeTool = std::ranges::find_if( m_mapTools, []( const auto& tool ) { return tool.second->IsActivated(); } );
	if ( activeTool != m_mapTools.end() )
		return activeTool->second.get();

	return nullptr;
}

bool ToolManager::SetupTools( SceneObject* pSceneObject, SCION_RENDERING::Camera2D* pCamera )
{
	for ( auto& [ eType, pTool ] : m_mapTools )
	{
		if ( !pTool->SetupTool( pSceneObject, pCamera ) )
			return false;
	}

	// TODO: Setup gizmos

	return true;
}

void ToolManager::SetToolsCurrentTileset( const std::string& sTileset )
{
	for ( auto& [ eType, pTool ] : m_mapTools )
	{
		pTool->LoadSpriteTextureData( sTileset );
	}
}

void ToolManager::SetTileToolStartCoords( int x, int y )
{
	for ( auto& [ eType, pTool ] : m_mapTools )
	{
		pTool->SetSpriteUVs( x, y );
	}
}

void ToolManager::EnableGridSnap( bool bEnable )
{
	for ( auto& tool : m_mapTools )
	{
		if ( bEnable )
			tool.second->EnableGridSnap();
		else
			tool.second->DisableGridSnap();
	}
}

} // namespace SCION_EDITOR
