#pragma once
#include "IDisplay.h"
#include <Core/ECS/Registry.h>

namespace SCION_EDITOR
{
class SceneDisplay : public IDisplay
{
  private:
	SCION_CORE::ECS::Registry& m_Registry;
	bool m_bPlayScene, m_bSceneLoaded;

  private:
	void LoadScene();
	void UnloadScene();

  public:
	SceneDisplay( SCION_CORE::ECS::Registry& registry );
	~SceneDisplay() = default;

	virtual void Draw() override;
	virtual void Update() override;
};
} // namespace SCION_EDITOR