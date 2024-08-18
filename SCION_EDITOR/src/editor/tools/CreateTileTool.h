#pragma once
#include "TileTool.h"

namespace SCION_EDITOR
{
class CreateTileTool : public TileTool
{
  private:
	void CreateNewTile();
	void RemoveTile();

  public:
	CreateTileTool();
	virtual ~CreateTileTool() = default;

	virtual void Create() override;
	virtual void Draw() override;
};
} // namespace SCION_EDITOR
