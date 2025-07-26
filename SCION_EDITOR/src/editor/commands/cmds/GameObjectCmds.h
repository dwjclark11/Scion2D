#pragma once
#include <memory>
#include <unordered_map>
#include <string>

namespace SCION_CORE
{
struct GameObjectData;
}

namespace SCION_EDITOR
{

class SceneObject;

struct AddGameObjectCmd
{
	SceneObject* pSceneObject{ nullptr };
	std::shared_ptr<SCION_CORE::GameObjectData> pGameObjectData{ nullptr };
	
	void undo();
	void redo();
};

struct DeleteGameObjectCmd
{
	SceneObject* pSceneObject{ nullptr };
	std::unordered_map <std::string, std::shared_ptr<SCION_CORE::GameObjectData>> mapTagToGameObjectData;

	void undo();
	void redo();
};

}
