#pragma once
#include "IDisplay.h"
#include <string>
#include <glm/glm.hpp>

namespace SCION_RENDERING
{
class Camera2D;
}

namespace SCION_CORE::Events
{
struct KeyEvent;
} // namespace SCION_CORE::Events

namespace SCION_EDITOR
{
class TilemapDisplay : public IDisplay
{
  private:
	std::unique_ptr<SCION_RENDERING::Camera2D> m_pTilemapCam;
	bool m_bWindowActive;

  private:
	void RenderTilemap();
	void LoadNewScene();
	void PanZoomCamera( const glm::vec2& mousePos );

	void HandleKeyPressedEvent( const SCION_CORE::Events::KeyEvent& keyEvent );

  protected:
	virtual void DrawToolbar() override;

  public:
	TilemapDisplay();
	virtual ~TilemapDisplay();

	virtual void Draw() override;
	virtual void Update() override;
};
} // namespace SCION_EDITOR
