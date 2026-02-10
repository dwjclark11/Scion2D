#pragma once
#include "TileTool.h"

namespace Scion::Editor
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
} // namespace Scion::Editor
