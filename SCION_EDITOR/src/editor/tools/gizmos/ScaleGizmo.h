#pragma once
#include "Gizmo.h"

namespace SCION_EDITOR
{
class ScaleGizmo : public Gizmo
{
  public:
	ScaleGizmo();
	virtual void Update( struct Canvas& canvas ) override;
	virtual void Draw() override;
};
} // namespace SCION_EDITOR
