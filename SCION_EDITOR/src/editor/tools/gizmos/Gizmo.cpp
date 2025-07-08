#include "Gizmo.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/Events/EventDispatcher.h"

#include "editor/utilities/EditorUtilities.h"
#include "editor/events/EditorEventTypes.h"

#include "Logger/Logger.h"

#include "Rendering/Core/BatchRenderer.h"
#include "Rendering/Essentials/Vertex.h"
#include <Rendering/Essentials/Texture.h>

using namespace SCION_CORE::ECS;

namespace SCION_EDITOR
{

Gizmo::Gizmo()
	: Gizmo( GizmoAxisParams{ .transform = TransformComponent{ .scale = glm::vec2{ 1.f, 0.5f } },
							  .axisColor = XAXIS_GIZMO_COLOR,
							  .axisHoveredColor = XAXIS_HOVERED_GIZMO_COLOR,
							  .axisDisabledColor = GRAYED_OUT_GIZMO_COLOR },
			 GizmoAxisParams{ .transform = TransformComponent{ .scale = glm::vec2{ 0.5f, 1.f } },
							  .axisOffset = glm::vec2{ -4.f },
							  .axisColor = YAXIS_GIZMO_COLOR,
							  .axisHoveredColor = YAXIS_HOVERED_GIZMO_COLOR,
							  .axisDisabledColor = GRAYED_OUT_GIZMO_COLOR },
			 false )
{
}

Gizmo::Gizmo( const GizmoAxisParams& xAxisParams, const GizmoAxisParams& yAxisParams, bool bOneAxis )
	: AbstractTool()
	, m_pXAxisParams{ nullptr }
	, m_pYAxisParams{ nullptr }
	, m_pBatchRenderer{ std::make_unique<SCION_RENDERING::SpriteBatchRenderer>() }
	, m_SelectedEntity{ entt::null }
	, m_LastMousePos{ 0.f }
	, m_bOverXAxis{ false }
	, m_bOverYAxis{ false }
	, m_bHoldingX{ false }
	, m_bHoldingY{ false }
	, m_bHidden{ false }
	, m_bOnlyOneAxis{ bOneAxis }
	, m_bUIComponent{ false }
{
	ADD_EVENT_HANDLER( SCION_EDITOR::Events::AddComponentEvent, &Gizmo::OnAddComponent, *this )

	m_pXAxisParams = std::make_unique<GizmoAxisParams>( xAxisParams );

	if ( m_bOnlyOneAxis )
		return;

	m_pYAxisParams = std::make_unique<GizmoAxisParams>( yAxisParams );
}

Gizmo::~Gizmo()
{
}

void Gizmo::Update( SCION_CORE::Canvas& canvas )
{
	m_LastMousePos = GetMouseScreenCoords();
	AbstractTool::Update( canvas );
}

void Gizmo::SetSelectedEntity( entt::entity entity )
{
	m_SelectedEntity = entity;

	if ( m_SelectedEntity != entt::null && m_pRegistry )
	{
		Entity ent{ *m_pRegistry, entity };
		SetGizmoPosition( ent );
		GetDispatcher().EmitEvent( Events::SwitchEntityEvent{ .pEntity = &ent } );
		m_bUIComponent = ent.HasComponent<UIComponent>();
	}
}

void Gizmo::Hide()
{
	if ( m_pXAxisParams )
		m_pXAxisParams->sprite.bHidden = true;

	if ( m_pYAxisParams )
		m_pYAxisParams->sprite.bHidden = true;

	m_bHidden = true;
}

void Gizmo::Show()
{
	if ( m_bHidden && m_SelectedEntity != entt::null )
	{
		if ( m_pXAxisParams )
			m_pXAxisParams->sprite.bHidden = false;

		if ( m_pYAxisParams )
			m_pYAxisParams->sprite.bHidden = false;

		m_bHidden = false;
	}
}

SCION_CORE::Events::EventDispatcher& Gizmo::GetDispatcher()
{
	if ( !m_pEventDispatcher )
		m_pEventDispatcher = std::make_unique<SCION_CORE::Events::EventDispatcher>();

	SCION_ASSERT( m_pEventDispatcher && "Event Dispatcher must be valid" );

	return *m_pEventDispatcher;
}

void Gizmo::Init( const std::string& sXAxisTexture, const std::string& sYAxisTexture )
{
	// Setup x-axis
	m_pXAxisParams->sprite.sTextureName = sXAxisTexture;
	auto pXAxisTexture = MAIN_REGISTRY().GetAssetManager().GetTexture( sXAxisTexture );
	SCION_ASSERT( pXAxisTexture && "Texture must exist!" );
	m_pXAxisParams->sprite.width = pXAxisTexture->GetWidth();
	m_pXAxisParams->sprite.height = pXAxisTexture->GetHeight();
	m_pXAxisParams->sprite.color = m_pXAxisParams->axisColor;

	SCION_CORE::GenerateUVs( m_pXAxisParams->sprite, pXAxisTexture->GetWidth(), pXAxisTexture->GetHeight() );

	if ( !m_bOnlyOneAxis )
	{
		// Setup y-axis
		m_pYAxisParams->sprite.sTextureName = sYAxisTexture;
		auto pYAxisTexture = MAIN_REGISTRY().GetAssetManager().GetTexture( sYAxisTexture );
		SCION_ASSERT( pYAxisTexture && "Texture must exist!" );
		m_pYAxisParams->sprite.width = pYAxisTexture->GetWidth();
		m_pYAxisParams->sprite.height = pYAxisTexture->GetHeight();
		m_pYAxisParams->sprite.color = m_pYAxisParams->axisColor;

		SCION_CORE::GenerateUVs( m_pYAxisParams->sprite, pYAxisTexture->GetWidth(), pYAxisTexture->GetHeight() );
	}

	Hide();
}

void Gizmo::ExamineMousePosition()
{
	glm::vec2 mousePos{ 0.f };
	if ( m_bUIComponent )
	{
		mousePos = GetMouseScreenCoords();
		const auto& guiWindowSize = GetWindowSize();
		auto& coreGlobals = CORE_GLOBALS();
		const int windowWidth = coreGlobals.WindowWidth();
		const int windowHeight = coreGlobals.WindowHeight();
		const float ratioX = windowWidth / guiWindowSize.x;
		const float ratioY = windowHeight / guiWindowSize.y;
		mousePos.x *= ratioX;
		mousePos.y *= ratioY;
	}
	else
	{
		mousePos = GetMouseWorldCoords();
	}

	// Check to see if we are hovering the x-axis of the gizmo
	const auto& xAxisTransform = m_pXAxisParams->transform;
	auto& xAxisSprite = m_pXAxisParams->sprite;

	if ( !m_bHoldingY && mousePos.x >= xAxisTransform.position.x &&
		 mousePos.x <= ( xAxisTransform.position.x + ( xAxisSprite.width * xAxisTransform.scale.x ) ) &&
		 mousePos.y >= xAxisTransform.position.y &&
		 mousePos.y <= ( xAxisTransform.position.y + ( xAxisSprite.height * xAxisTransform.scale.y ) ) )
	{
		xAxisSprite.color = m_pXAxisParams->axisHoveredColor;
		m_bOverXAxis = true;
	}
	else if ( m_bHoldingX )
	{
		xAxisSprite.color = m_pXAxisParams->axisHoveredColor;
		m_bOverXAxis = true;
	}
	else if ( m_bHoldingY )
	{
		xAxisSprite.color = m_pXAxisParams->axisDisabledColor;
		m_bOverXAxis = false;
	}
	else
	{
		xAxisSprite.color = m_pXAxisParams->axisColor;
		m_bOverXAxis = false;
	}

	if ( m_bOnlyOneAxis )
		return;

	// Check to see if we are hovering the y-axis of the gizmo
	const auto& yAxisTransform = m_pYAxisParams->transform;
	auto& yAxisSprite = m_pYAxisParams->sprite;

	if ( !m_bHoldingX && mousePos.x >= yAxisTransform.position.x &&
		 mousePos.x <= ( yAxisTransform.position.x + ( yAxisSprite.width * yAxisTransform.scale.x ) ) &&
		 mousePos.y >= yAxisTransform.position.y &&
		 mousePos.y <= ( yAxisTransform.position.y + ( yAxisSprite.height * yAxisTransform.scale.y ) ) )
	{
		yAxisSprite.color = m_pYAxisParams->axisHoveredColor;
		m_bOverYAxis = true;
	}
	else if ( m_bHoldingY )
	{
		yAxisSprite.color = m_pYAxisParams->axisHoveredColor;
		m_bOverYAxis = true;
	}
	else if ( m_bHoldingX )
	{
		yAxisSprite.color = m_pYAxisParams->axisDisabledColor;
		m_bOverYAxis = false;
	}
	else
	{
		yAxisSprite.color = m_pYAxisParams->axisColor;
		m_bOverYAxis = false;
	}
}

float Gizmo::GetDeltaX()
{
	if ( !IsOverTilemapWindow() || m_bHoldingY )
		return 0.f;

	if ( !m_bOverXAxis && !m_bHoldingX )
		return 0.f;

	if ( MouseBtnPressed( AbstractTool::EMouseButton::LEFT ) && MouseMoving() )
	{
		m_bHoldingX = true;
		if ( m_bUIComponent )
		{
			float mousePosX = GetMouseScreenCoords().x;
			const auto& guiWindowSize = GetWindowSize();
			auto& coreGlobals = CORE_GLOBALS();
			const int windowWidth = coreGlobals.WindowWidth();
			const float ratioX = windowWidth / guiWindowSize.x;
			mousePosX *= ratioX;
			m_LastMousePos.x *= ratioX;

			return mousePosX - m_LastMousePos.x;
		}
		else
		{
			return ( GetMouseScreenCoords().x - m_LastMousePos.x ) / m_pCamera->GetScale();
		}
	}

	if ( MouseBtnJustReleased( AbstractTool::EMouseButton::LEFT ) )
	{
		m_bHoldingX = false;
	}

	return 0.f;
}

float Gizmo::GetDeltaY()
{
	if ( !IsOverTilemapWindow() || m_bOnlyOneAxis || m_bHoldingX )
		return 0.f;

	if ( !m_bOverYAxis && !m_bHoldingY )
		return 0.f;

	if ( MouseBtnPressed( AbstractTool::EMouseButton::LEFT ) && MouseMoving() )
	{
		m_bHoldingY = true;
		if ( m_bUIComponent )
		{
			float mousePosY = GetMouseScreenCoords().y;
			const auto& guiWindowSize = GetWindowSize();
			auto& coreGlobals = CORE_GLOBALS();
			const int windowHeight = coreGlobals.WindowHeight();
			const float ratioY = windowHeight / guiWindowSize.y;
			mousePosY *= ratioY;
			m_LastMousePos.y *= ratioY;

			return mousePosY - m_LastMousePos.y;
		}
		else
		{
			return ( GetMouseScreenCoords().y - m_LastMousePos.y ) / m_pCamera->GetScale();
		}
	}

	if ( MouseBtnJustReleased( AbstractTool::EMouseButton::LEFT ) )
	{
		m_bHoldingY = false;
	}

	return 0.f;
}

void Gizmo::SetGizmoPosition( SCION_CORE::ECS::Entity& selectedEntity )
{
	float spriteWidth{ 0.f };
	float spriteHeight{ 0.f };

	if ( const auto* sprite = selectedEntity.TryGetComponent<SpriteComponent>() )
	{
		spriteWidth = sprite->width;
		spriteHeight = sprite->height;
	}

	const auto& selectedTransform = selectedEntity.GetComponent<TransformComponent>();
	if ( !m_bOnlyOneAxis )
	{
		m_pXAxisParams->transform.position =
			selectedTransform.position + m_pXAxisParams->axisOffset +
			glm::vec2{ ( spriteWidth * selectedTransform.scale.x * 0.5f ),
					   ( spriteHeight * selectedTransform.scale.y * 0.5f ) -
						   ( m_pXAxisParams->sprite.height * m_pXAxisParams->transform.scale.y * 0.5f ) };

		m_pYAxisParams->transform.position =
			selectedTransform.position + m_pYAxisParams->axisOffset +
			glm::vec2{ ( spriteWidth * selectedTransform.scale.x * 0.5f ) -
						   ( m_pYAxisParams->sprite.width * m_pYAxisParams->transform.scale.x * 0.5f ),
					   ( spriteHeight * selectedTransform.scale.y * 0.5f ) -
						   ( m_pYAxisParams->sprite.height * m_pYAxisParams->transform.scale.y ) };
	}
	else
	{
		m_pXAxisParams->transform.position =
			selectedTransform.position + m_pXAxisParams->axisOffset +
			glm::vec2{ ( spriteWidth * selectedTransform.scale.x * 0.5f ) -
						   ( m_pXAxisParams->sprite.width * m_pXAxisParams->transform.scale.x * 0.5f ),
					   ( spriteHeight * selectedTransform.scale.y * 0.5f ) -
						   ( m_pXAxisParams->sprite.height * m_pXAxisParams->transform.scale.y * 0.5f ) };
	}
}

void Gizmo::OnAddComponent( const SCION_EDITOR::Events::AddComponentEvent& addCompEvent )
{
	if ( addCompEvent.eType == Events::EComponentType::UI || addCompEvent.eType == Events::EComponentType::Text )
	{
		m_bUIComponent = true;
	}
}

} // namespace SCION_EDITOR
