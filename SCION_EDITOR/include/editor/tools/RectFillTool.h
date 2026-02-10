#pragma once
#include "TileTool.h"

namespace Scion::Rendering
{
class RectBatchRenderer;
struct Rect;
} // namespace Scion::Rendering

namespace Scion::Editor
{
class RectFillTool : public TileTool
{
  private:
	std::unique_ptr<Scion::Rendering::RectBatchRenderer> m_pShapeRenderer;
	std::unique_ptr<Scion::Rendering::Rect> m_pTileFillRect;
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
} // namespace Scion::Editor
