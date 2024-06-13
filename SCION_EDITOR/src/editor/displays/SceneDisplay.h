#pragma once
#include <Core/ECS/Registry.h>

namespace SCION_EDITOR
{
class SceneDisplay
{
  private:
	SCION_CORE::ECS::Registry& m_Registry;

  public:
	SceneDisplay( SCION_CORE::ECS::Registry& registry );
	~SceneDisplay() = default;

	void Draw();
};
} // namespace SCION_EDITOR