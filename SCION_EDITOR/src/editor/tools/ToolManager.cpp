#include "ToolManager.h"
#include "CreateTileTool.h"
#include "RectFillTool.h"
#include "ToolAccessories.h"
#include "editor/tools/gizmos/TranslateGizmo.h"
#include "editor/tools/gizmos/ScaleGizmo.h"
#include "editor/tools/gizmos/RotateGizmo.h"
#include "editor/scene/SceneObject.h"
#include "Rendering/Core/Camera2D.h"

namespace SCION_EDITOR
{
ToolManager::ToolManager()
{
	m_mapTools.emplace( EToolType::CREATE_TILE, std::make_unique<CreateTileTool>() );
	m_mapTools.emplace( EToolType::RECT_FILL_TILE, std::make_unique<RectFillTool>() );

	m_mapGizmos.emplace( EGizmoType::TRANSLATE, std::make_unique<TranslateGizmo>() );
	m_mapGizmos.emplace( EGizmoType::SCALE, std::make_unique<ScaleGizmo>() );
	m_mapGizmos.emplace( EGizmoType::ROTATE, std::make_unique<RotateGizmo>() );

	// TODO: Add other tools as needed.

	SetToolActive( EToolType::RECT_FILL_TILE );
}

void ToolManager::Update( Canvas& canvas )
{
	auto activeTool = std::ranges::find_if( m_mapTools, []( const auto& tool ) { return tool.second->IsActivated(); } );
	if ( activeTool != m_mapTools.end() )
		activeTool->second->Update( canvas );

	auto activeGizmo =
		std::ranges::find_if( m_mapGizmos, []( const auto& gizmo ) { return gizmo.second->IsActivated(); } );
	if ( activeGizmo != m_mapGizmos.end() )
		activeGizmo->second->Update( canvas );
}

void ToolManager::SetToolActive( EToolType eToolType )
{
	// Deactivate all gizmos when map exists
	for ( const auto& [ eType, pGizmo ] : m_mapGizmos )
	{
		pGizmo->Deactivate();
		pGizmo->Hide();
	}

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
	for ( const auto& [ eType, pGizmo ] : m_mapGizmos )
	{
		if ( eType == eGizmoType )
		{
			pGizmo->Activate();
			pGizmo->Show();
		}
		else
		{
			pGizmo->Deactivate();
			pGizmo->Hide();
		}
	}

	m_eActiveGizmoType = eGizmoType;
}

TileTool* ToolManager::GetActiveTool()
{
	auto activeTool = std::ranges::find_if( m_mapTools, []( const auto& tool ) { return tool.second->IsActivated(); } );
	if ( activeTool != m_mapTools.end() )
		return activeTool->second.get();

	return nullptr;
}

Gizmo* ToolManager::GetActiveGizmo()
{
	auto activeGizmo =
		std::ranges::find_if( m_mapGizmos, []( const auto& gizmo ) { return gizmo.second->IsActivated(); } );
	if ( activeGizmo != m_mapGizmos.end() )
		return activeGizmo->second.get();

	return nullptr;
}

AbstractTool* ToolManager::GetActiveToolFromAbstract()
{
	if ( auto* pTool = GetActiveTool() )
		return pTool;

	if ( auto* pGizmo = GetActiveGizmo() )
		return pGizmo;

	return nullptr;
}

bool ToolManager::SetupTools( SceneObject* pSceneObject, SCION_RENDERING::Camera2D* pCamera )
{
	for ( auto& [ eType, pTool ] : m_mapTools )
	{
		if ( !pTool->SetupTool( pSceneObject, pCamera ) )
			return false;
	}

	for ( auto& [ eType, pGizmo] : m_mapGizmos )
	{
		if ( !pGizmo->SetupTool( pSceneObject, pCamera ) )
			return false;
	}

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

void ToolManager::SetSelectedEntity( entt::entity entity )
{
	for (auto& [eGizmo, pGizmo] : m_mapGizmos)
	{
		pGizmo->SetSelectedEntity( entity );
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
