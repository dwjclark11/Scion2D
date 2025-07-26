#pragma once
#include "editor/tools/AbstractTool.h"
#include <memory>
#include <entt/entt.hpp>

namespace SCION_CORE
{

namespace ECS
{
class Entity;
}

} // namespace SCION_CORE

namespace SCION_RENDERING
{
class SpriteBatchRenderer;
}

namespace SCION_CORE::Events
{
class EventDispatcher;
} // namespace SCION_CORE::Events

namespace SCION_EDITOR
{
namespace Events
{
struct AddComponentEvent;
struct EntityDeletedEvent;
}

struct GizmoAxisParams;

class Gizmo : public AbstractTool
{
  public:
	Gizmo();
	Gizmo( const GizmoAxisParams& xAxisParams, const GizmoAxisParams& yAxisParams, bool bOneAxis );
	virtual ~Gizmo();

	virtual void Update( SCION_CORE::Canvas& canvas ) override;
	virtual void Draw( SCION_RENDERING::Camera2D* pCamera ) = 0;

	void SetSelectedEntity( entt::entity entity );
	void Hide();
	void Show();

	SCION_CORE::Events::EventDispatcher& GetDispatcher();
	inline bool OverGizmo() const { return !( !m_bHoldingX && !m_bHoldingY && !m_bOverXAxis && !m_bOverYAxis ); }

  protected:
	void Init( const std::string& sXAxisTexture, const std::string& sYAxisTexture );
	virtual void ExamineMousePosition() override;

	float GetDeltaX();
	float GetDeltaY();

	void SetGizmoPosition( SCION_CORE::ECS::Entity& selectedEntity );
	virtual void ResetSelectedEntity() override { m_SelectedEntity = entt::null; }

  protected:
	std::unique_ptr<GizmoAxisParams> m_pXAxisParams;
	std::unique_ptr<GizmoAxisParams> m_pYAxisParams;
	std::unique_ptr<SCION_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;
	std::unique_ptr<SCION_CORE::Events::EventDispatcher> m_pEventDispatcher{ nullptr };

	entt::entity m_SelectedEntity;
	glm::vec2 m_LastMousePos;

	bool m_bOverXAxis;
	bool m_bOverYAxis;
	bool m_bHoldingX;
	bool m_bHoldingY;
	bool m_bHidden;
	bool m_bOnlyOneAxis;
	bool m_bUIComponent;

  private:
	void OnAddComponent( const SCION_EDITOR::Events::AddComponentEvent& addCompEvent );
	void OnEntityDeleted( const SCION_EDITOR::Events::EntityDeletedEvent& deleteEntityEvent );
};
} // namespace SCION_EDITOR
