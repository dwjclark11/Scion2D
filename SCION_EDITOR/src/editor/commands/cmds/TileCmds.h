#pragma once
#include "ScionUtilities/HelperUtilities.h"

namespace SCION_CORE::ECS
{
class Registry;
}

namespace SCION_EDITOR
{
struct Tile;
class SceneObject;

struct RemoveTileLayerCmd
{
	SceneObject* pSceneObject{ nullptr };
	std::vector<Tile> tilesRemoved;
	SCION_UTIL::SpriteLayerParams spriteLayerParams;

	void undo();
	void redo();
};

struct AddTileLayerCmd
{
	SceneObject* pSceneObject{ nullptr };
	SCION_UTIL::SpriteLayerParams spriteLayerParams;

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

} // namespace SCION_EDITOR
