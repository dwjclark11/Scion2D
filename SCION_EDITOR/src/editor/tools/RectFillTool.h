#pragma once
#include "TileTool.h"

namespace SCION_RENDERING
{
class RectBatchRenderer;
struct Rect;
} // namespace SCION_RENDERING

namespace SCION_EDITOR
{
class RectFillTool : public TileTool
{
  private:
	std::unique_ptr<SCION_RENDERING::RectBatchRenderer> m_pShapeRenderer;
	std::unique_ptr<SCION_RENDERING::Rect> m_pTileFillRect;
	glm::vec2 m_StartPressPos;

  private:
	void CreateTiles();
	void RemoveTiles();
	void DrawPreview( int dx, int dy );
	void ResetTile();

  public:
	RectFillTool();
	~RectFillTool();

	virtual void Create() override;
	virtual void Draw() override;
};
} // namespace SCION_EDITOR
