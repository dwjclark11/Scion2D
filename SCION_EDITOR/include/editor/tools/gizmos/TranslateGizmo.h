#pragma once
#include "Gizmo.h"

namespace Scion::Editor
{
class TranslateGizmo : public Gizmo
{
  public:
	TranslateGizmo();
	virtual void Update( Scion::Core::Canvas& canvas ) override;
	virtual void Draw( Scion::Rendering::Camera2D* pCamera ) override;
};
} // namespace Scion::Editor
