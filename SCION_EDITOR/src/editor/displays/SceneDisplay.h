#pragma once
#include "IDisplay.h"
#include <Core/ECS/Registry.h>

namespace SCION_CORE::Events
{
struct KeyEvent;
}

namespace SCION_EDITOR
{
class SceneDisplay : public IDisplay
{
  public:
	SceneDisplay();
	~SceneDisplay() = default;

	virtual void Draw() override;
	virtual void Update() override;

  protected:
	virtual void DrawToolbar() override;

  private:
	void LoadScene();
	void UnloadScene();
	void RenderScene() const;

	void HandleKeyEvent( const SCION_CORE::Events::KeyEvent keyEvent );

  private:
	bool m_bPlayScene;
	bool m_bWindowActive;
	bool m_bSceneLoaded;
};
} // namespace SCION_EDITOR
