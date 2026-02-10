#pragma once

namespace Scion::Core::ECS
{
class Registry;
}

namespace Scion::Editor
{
struct Tile;

struct RectToolAddTilesCmd
{
	Scion::Core::ECS::Registry* pRegistry{ nullptr };
	std::vector<Tile> tiles{};

	void undo();
	void redo();
};

struct RectToolRemoveTilesCmd
{
	Scion::Core::ECS::Registry* pRegistry{ nullptr };
	std::vector<Tile> tiles{};

	void undo();
	void redo();
};

} // namespace Scion::Editor
