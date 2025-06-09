#pragma once
#include "Gizmo.h"

namespace SCION_EDITOR
{
class TranslateGizmo : public Gizmo
{
  public:
	TranslateGizmo();
	virtual void Update( SCION_CORE::Canvas& canvas ) override;
	virtual void Draw( SCION_RENDERING::Camera2D* pCamera ) override;
};
} // namespace SCION_EDITOR
