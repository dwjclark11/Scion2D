#pragma once
#include <memory>

namespace SCION_CORE::ECS
{
class Registry;
}

namespace SCION_EDITOR
{
struct Tile;

struct CreateTileToolAddCmd
{
	SCION_CORE::ECS::Registry* pRegistry{ nullptr };
	std::shared_ptr<Tile> pTile{ nullptr };

	void undo();
	void redo();
};

}
