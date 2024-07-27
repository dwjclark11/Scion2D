#pragma once
#include "IDisplay.h"
#include <Core/ECS/Registry.h>

namespace SCION_EDITOR
{
class SceneDisplay : public IDisplay
{
  private:
	bool m_bPlayScene, m_bSceneLoaded;

  private:
	void LoadScene();
	void UnloadScene();
	void RenderScene();

  public:
	SceneDisplay();
	~SceneDisplay() = default;

	virtual void Draw() override;
	virtual void Update() override;
};
} // namespace SCION_EDITOR