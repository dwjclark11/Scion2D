#pragma once
#include "IDisplay.h"

namespace Scion::Core::Events
{
struct KeyEvent;
}

namespace Scion::Editor
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

	void HandleKeyEvent( const Scion::Core::Events::KeyEvent keyEvent );

  private:
	bool m_bPlayScene;
	bool m_bWindowActive;
	bool m_bSceneLoaded;
};
} // namespace Scion::Editor
