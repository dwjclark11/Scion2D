#pragma once
#include "Core/ECS/Entity.h"
#include "editor/utilities/EditorUtilities.h"
#include "ScionUtilities/HelperUtilities.h"

namespace SCION_EDITOR::Events
{
struct NameChangeEvent;
}

namespace SCION_EDITOR
{

class SceneObject
{
  public:
	SceneObject( const std::string& sceneName );
	SceneObject( const std::string& sceneName, const std::string& sceneData );
	~SceneObject() = default;

	/*
	 * @brief Copies the current tilemap and game objects to the the
	 * runtime registry. This is to be used when running a scene.
	 */
	void CopySceneToRuntime();

	void CopySceneToRuntime( SceneObject& sceneToCopy );

	/*
	 * @brief Clears the runtime registry.
	 */
	void ClearRuntimeScene();

	// Scene Tilemap Layer functions
	/*
	 * @brief Adds a new layer to the scene tilemap layers.
	 */
	void AddNewLayer();

	/*
	 * @brief Tries to add a specific layer and set it's visibility.
	 * If the layer already exists, it will do nothing.
	 * @param std::string for the layer name.
	 * @param bool to set the visibility of the layer.
	 */
	void AddLayer( const std::string& sLayerName, bool bVisible );

	/*
	 * @brief Checks to see if the layer already exists in the scene.
	 * To be used when trying to adjust the name of the layer to prevent layers with duplicate names.
	 * @param Takes in a string for the layer to check.
	 * @return Returns true if that layer does not exist, false otherwise.
	 */
	bool CheckLayerName( const std::string& sLayerName );

	bool AddGameObject();
	bool AddGameObjectByTag( const std::string& sTag, entt::entity entity );
	bool DuplicateGameObject( entt::entity entity );
	bool DeleteGameObjectByTag( const std::string& sTag );
	bool DeleteGameObjectById( entt::entity entity );

	/*
	 * @brief Tries to load the scene. Loads the tilemap, layers,
	 * game objects, and other scene data.
	 * @return Returns true if successful, false otherwise.
	 */
	bool LoadScene();

	/*
	 * @brief Tries to Unload the scene. When unloading, this will
	 * try to save the tilemap, layers, game objects, and other scene data.
	 * The scene's registry will also be cleared.
	 * @return Returns true if successful, false otherwise.
	 */
	bool UnloadScene(bool bSaveScene = true);

	/*
	 * @brief Tries to save the scene. This differs from the unload function
	 * because it does not set the loaded flag or clear the registry.
	 * @return Returns true if successful, false otherwise.
	 */
	bool SaveScene();

	bool CheckTagName( const std::string& sTagName );

	/*
	 * @brief Checks to see if there are any layers in the scenes tilemap.
	 * @return Returns true if there are layers present, false otherwise.
	 */
	inline bool HasTileLayers() const { return !m_LayerParams.empty(); }
	inline std::vector<SCION_UTIL::SpriteLayerParams>& GetLayerParams() { return m_LayerParams; }

	inline Canvas& GetCanvas() { return m_Canvas; }
	inline const std::string& GetName() { return m_sSceneName; }
	inline const std::string& GetRuntimeName() { return m_sRuntimeSceneName; }
	inline const std::string& GetSceneDataPath() { return m_sSceneDataPath; }
	inline SCION_CORE::ECS::Registry& GetRegistry() { return m_Registry; }
	inline SCION_CORE::ECS::Registry* GetRegistryPtr() { return &m_Registry; }
	inline SCION_CORE::ECS::Registry& GetRuntimeRegistry() { return m_RuntimeRegistry; }

	inline bool IsLoaded() const { return m_bSceneLoaded; }

  private:
	bool LoadSceneData();
	bool SaveSceneData();
	void OnEntityNameChanges( SCION_EDITOR::Events::NameChangeEvent& nameChange );

  private:
	/* The registry that is used in the tilemap editor and the scene hierarchy. */
	SCION_CORE::ECS::Registry m_Registry;
	/* The runtime registry. This registry starts with the current scene's data;
	however, different scenes can be loaded via lua scripts. */
	SCION_CORE::ECS::Registry m_RuntimeRegistry;
	/* The name of the scene object represents. */
	std::string m_sSceneName;
	/* The current scene that the runtime is using. Scene can be changed via lua script. */
	std::string m_sRuntimeSceneName;
	/* The filepath which to load and save the tilemap. */
	std::string m_sTilemapPath;
	/* The filepath which to load and save the game objects. */
	std::string m_sObjectPath;
	/* The filepath which to load and save the scene data. */
	std::string m_sSceneDataPath;

	Canvas m_Canvas;
	std::vector<SCION_UTIL::SpriteLayerParams> m_LayerParams;
	std::map<std::string, entt::entity> m_mapTagToEntity;
	int m_CurrentLayer;
	/* Has this scene been loaded in the editor. */
	bool m_bSceneLoaded;
};

} // namespace SCION_EDITOR
