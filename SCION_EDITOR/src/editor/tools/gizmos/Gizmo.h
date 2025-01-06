#pragma once
#include "editor/tools/AbstractTool.h"
#include <memory>
#include <entt/entt.hpp>

namespace SCION_CORE::ECS
{
class Entity;
}

namespace SCION_RENDERING
{
class SpriteBatchRenderer;
}

namespace SCION_EDITOR
{

struct GizmoAxisParams;
struct Canvas;

class Gizmo : public AbstractTool
{
  public:
	Gizmo();
	Gizmo( const GizmoAxisParams& xAxisParams, const GizmoAxisParams& yAxisParams, bool bOneAxis );

	virtual void Update( Canvas& canvas ) override;
	virtual void Draw() = 0;

	void SetSelectedEntity( entt::entity entity );
	void Hide();
	void Show();

	inline bool OverGizmo() const { return false; }

  protected:
	void Init( const std::string& sXAxisTexture, const std::string& sYAxisTexture );
	virtual void ExamineMousePosition() override;

	float GetDeltaX();
	float GetDeltaY();

	void SetGizmoPosition( SCION_CORE::ECS::Entity& selectedEntity );

  protected:
	std::unique_ptr<GizmoAxisParams> m_pXAxisParams;
	std::unique_ptr<GizmoAxisParams> m_pYAxisParams;
	std::unique_ptr<SCION_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;

	entt::entity m_SelectedEntity;
	glm::vec2 m_LastMousePos;

	bool m_bOverXAxis;
	bool m_bOverYAxis;
	bool m_bHoldingX;
	bool m_bHoldingY;
	bool m_bHidden;
	bool m_bOnlyOneAxis;

};
} // namespace SCION_EDITOR
