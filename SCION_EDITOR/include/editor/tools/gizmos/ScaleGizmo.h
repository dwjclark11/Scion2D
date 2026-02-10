#pragma once
#include "Gizmo.h"

namespace Scion::Editor
{
class ScaleGizmo : public Gizmo
{
  public:
	ScaleGizmo();
	virtual void Update( Scion::Core::Canvas& canvas ) override;
	virtual void Draw( Scion::Rendering::Camera2D* pCamera ) override;
};
} // namespace Scion::Editor
