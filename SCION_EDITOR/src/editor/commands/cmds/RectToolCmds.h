#pragma once

namespace SCION_CORE::ECS
{
class Registry;
}

namespace SCION_EDITOR
{
struct Tile;

struct RectToolAddTilesCmd
{
	SCION_CORE::ECS::Registry* pRegistry{ nullptr };
	std::vector<Tile> tiles{};

	void undo();
	void redo();
};

struct RectToolRemoveTilesCmd
{
	SCION_CORE::ECS::Registry* pRegistry{ nullptr };
	std::vector<Tile> tiles{};

	void undo();
	void redo();
};

} // namespace SCION_EDITOR
