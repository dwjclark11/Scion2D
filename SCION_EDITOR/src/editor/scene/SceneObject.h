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
	bool UnloadScene();

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
	inline const std::string& GetSceneDataPath() { return m_sSceneDataPath; }
	inline SCION_CORE::ECS::Registry& GetRegistry() { return m_Registry; }
	inline SCION_CORE::ECS::Registry* GetRegistryPtr() { return &m_Registry; }
	inline SCION_CORE::ECS::Registry& GetRuntimeRegistry() { return m_RuntimeRegistry; }

  private:
	bool LoadSceneData();
	bool SaveSceneData();
	void OnEntityNameChanges( SCION_EDITOR::Events::NameChangeEvent& nameChange );

  private:
	SCION_CORE::ECS::Registry m_Registry;
	SCION_CORE::ECS::Registry m_RuntimeRegistry;

	std::string m_sSceneName;
	std::string m_sTilemapPath;
	std::string m_sObjectPath;
	std::string m_sSceneDataPath;

	Canvas m_Canvas;
	std::vector<SCION_UTIL::SpriteLayerParams> m_LayerParams;
	std::map<std::string, entt::entity> m_mapTagToEntity;
	int m_CurrentLayer;
	bool m_bSceneLoaded{ false };
};

} // namespace SCION_EDITOR
