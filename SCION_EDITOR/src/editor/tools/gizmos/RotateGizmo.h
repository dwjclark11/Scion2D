#pragma once
#include "Gizmo.h"

namespace SCION_EDITOR
{
class RotateGizmo : public Gizmo
{
  public:
	RotateGizmo();
	virtual void Update( SCION_CORE::Canvas& canvas ) override;
	virtual void Draw( SCION_RENDERING::Camera2D* pCamera ) override;
};
} // namespace SCION_EDITOR
