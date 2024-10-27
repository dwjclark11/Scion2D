#include "AbstractTool.h"
#include "Logger/Logger.h"
#include "Core/Scripting/InputManager.h"
#include "Core/ECS/Registry.h"
#include "Rendering/Core/Camera2D.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/scene/SceneObject.h"

namespace SCION_EDITOR
{

void AbstractTool::UpdateMouseWorldCoords()
{
	m_MouseScreenCoords = m_GUICursorCoords - m_GUIRelativeCoords;
	if ( !m_pCamera )
		return;

	m_MouseWorldCoords = m_pCamera->ScreenCoordsToWorld( m_MouseScreenCoords );
}

void AbstractTool::CheckOutOfBounds( const Canvas& canvas )
{
	auto boundsWidth{ canvas.width - ( canvas.tileWidth * 0.5f ) };
	auto boundsHeight{ canvas.height - ( canvas.tileHeight * 0.5f ) };

	if ( m_MouseWorldCoords.x <= boundsWidth && m_MouseWorldCoords.y <= boundsHeight && m_MouseWorldCoords.x >= 0.f &&
		 m_MouseWorldCoords.y >= 0.f )
	{
		m_bOutOfBounds = false;
	}
	else
	{
		m_bOutOfBounds = true;
	}
}

bool AbstractTool::MouseBtnJustPressed( EMouseButton eButton )
{
	return INPUT_MANAGER().GetMouse().IsBtnJustPressed( static_cast<int>( eButton ) );
}

bool AbstractTool::MouseBtnJustReleased( EMouseButton eButton )
{
	return INPUT_MANAGER().GetMouse().IsBtnJustReleased( static_cast<int>( eButton ) );
}

bool AbstractTool::MouseBtnPressed( EMouseButton eButton )
{
	return INPUT_MANAGER().GetMouse().IsBtnPressed( static_cast<int>( eButton ) );
}

bool AbstractTool::MouseMoving()
{
	return INPUT_MANAGER().GetMouse().IsMouseMoving();
}

AbstractTool::AbstractTool()
	: m_MouseScreenCoords{ 0.f }
	, m_MouseWorldCoords{ 0.f }
	, m_GUICursorCoords{ 0.f }
	, m_GUIRelativeCoords{ 0.f }
	, m_WindowPos{ 0.f }
	, m_WindowSize{ 0.f }
	, m_bActivated{ false }
	, m_bOutOfBounds{ false }
	, m_bOverTilemapWindow{ false }
{
}

void AbstractTool::Update( Canvas& canvas )
{
	UpdateMouseWorldCoords();
	CheckOutOfBounds( canvas );
}

bool AbstractTool::SetupTool( SceneObject* pSceneObject, SCION_RENDERING::Camera2D* pCamera )
{
	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to setup tool - SceneObject was nullptr." );
		return false;
	}

	if ( !pSceneObject->GetRegistryPtr() )
	{
		SCION_ERROR( "Failed to setup tool - Registry was nullptr." );
		return false;
	}

	if ( !pCamera )
	{
		SCION_ERROR( "Failed to setup tool - Camera was nullptr." );
		return false;
	}
	m_pCurrentScene = pSceneObject;
	m_pCamera = pCamera;
	m_pRegistry = pSceneObject->GetRegistryPtr();

	return true;
}

} // namespace SCION_EDITOR
