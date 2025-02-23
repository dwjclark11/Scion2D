#pragma once
#include "Core/Scene/Scene.h"
#include "editor/utilities/EditorUtilities.h"

namespace SCION_EDITOR::Events
{
struct NameChangeEvent;
}

namespace SCION_EDITOR
{

class SceneObject : public SCION_CORE::Scene
{
  public:
	SceneObject( const std::string& sceneName, SCION_CORE::EMapType eType = SCION_CORE::EMapType::Grid );
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

	bool AddGameObject();
	bool AddGameObjectByTag( const std::string& sTag, entt::entity entity );
	bool DuplicateGameObject( entt::entity entity );
	bool DeleteGameObjectByTag( const std::string& sTag );
	bool DeleteGameObjectById( entt::entity entity );

	virtual bool LoadScene() override;
	virtual bool UnloadScene( bool bSaveScene = true ) override;
	

	bool CheckTagName( const std::string& sTagName );

	inline const std::string& GetName() { return m_sSceneName; }
	inline const std::string& GetRuntimeName() { return m_sRuntimeSceneName; }
	inline SCION_CORE::ECS::Registry& GetRuntimeRegistry() { return m_RuntimeRegistry; }

  private:
	void OnEntityNameChanges( SCION_EDITOR::Events::NameChangeEvent& nameChange );
	

  private:
	/* The runtime registry. This registry starts with the current scene's data;
	however, different scenes can be loaded via lua scripts. */
	SCION_CORE::ECS::Registry m_RuntimeRegistry;
	/* The current scene that the runtime is using. Scene can be changed via lua script. */
	std::string m_sRuntimeSceneName;
	std::map<std::string, entt::entity> m_mapTagToEntity;
	int m_CurrentLayer;
};

} // namespace SCION_EDITOR
