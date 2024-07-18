#pragma once
#include "IDisplay.h"
#include <string>

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

  public:
	TilemapDisplay();
	~TilemapDisplay() = default;

	virtual void Draw() override;
	virtual void Update() override;
};
} // namespace SCION_EDITOR