#pragma once
#include "ScionUtilities/HelperUtilities.h"

namespace Scion::Core::ECS
{
class Registry;
}

namespace Scion::Editor
{
struct Tile;
class SceneObject;

struct RemoveTileLayerCmd
{
	SceneObject* pSceneObject{ nullptr };
	std::vector<Tile> tilesRemoved;
	Scion::Utilities::SpriteLayerParams spriteLayerParams;

	void undo();
	void redo();
};

struct AddTileLayerCmd
{
	SceneObject* pSceneObject{ nullptr };
	Scion::Utilities::SpriteLayerParams spriteLayerParams;

	void undo();
	void redo();
};

struct MoveTileLayerCmd
{
	SceneObject* pSceneObject{ nullptr };
	int from;
	int to;

	void undo();
	void redo();
};

struct ChangeTileLayerNameCmd
{
	SceneObject* pSceneObject{ nullptr };
	std::string sOldName{};
	std::string sNewName{};

	void undo();
	void redo();
};

} // namespace Scion::Editor
