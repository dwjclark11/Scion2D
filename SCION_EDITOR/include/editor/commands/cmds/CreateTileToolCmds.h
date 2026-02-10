#pragma once

namespace Scion::Core::ECS
{
class Registry;
}

namespace Scion::Editor
{
struct Tile;

struct CreateTileToolAddCmd
{
	Scion::Core::ECS::Registry* pRegistry{ nullptr };
	std::shared_ptr<Tile> pTile{ nullptr };

	void undo();
	void redo();
};

struct CreateTileToolRemoveCmd
{
	Scion::Core::ECS::Registry* pRegistry{ nullptr };
	std::shared_ptr<Tile> pTile{ nullptr };

	void undo();
	void redo();
};

} // namespace Scion::Editor
