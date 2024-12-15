#pragma once
#include "IDisplay.h"
#include <string>
#include <glm/glm.hpp>

namespace SCION_RENDERING
{
class Camera2D;
}

namespace SCION_EDITOR
{
class TilemapDisplay : public IDisplay
{
  private:
	std::unique_ptr<SCION_RENDERING::Camera2D> m_pTilemapCam;

  private:
	void RenderTilemap();
	void LoadNewScene();
	void PanZoomCamera( const glm::vec2& mousePos );

  protected:
	virtual void DrawToolbar() override;

  public:
	TilemapDisplay();
	virtual ~TilemapDisplay();

	virtual void Draw() override;
	virtual void Update() override;
};
} // namespace SCION_EDITOR
