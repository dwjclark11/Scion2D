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

	/**
	 * @brief Copies the passed in scene to the current scene's runtime registry.
	 *
	 * @param sceneToCopy - A reference to the desired scene to copy.
	 */
	void CopySceneToRuntime( SceneObject& sceneToCopy );

	/*
	* @brief Each scene might have it's own player start. So when changing the
	* scenes while the scene is running, we need to copy to the current scenes
	* runtime. We don't want to actually change the scenes. This will copy the
	* player start from the desired scene to the current scenes registry.
	* @param runtimeRegistry - The desired registry to copy the player start to.
	*/
	void CopyPlayerStartToRuntimeRegistry( SCION_CORE::ECS::Registry& runtimeRegistry );

	/*
	 * @brief Clears the runtime registry.
	 */
	void ClearRuntimeScene();

	// Scene Tilemap Layer functions
	/*
	 * @brief Adds a new layer to the scene tilemap layers.
	 */
	void AddNewLayer();

	/**
	 * @brief Adds a new game object to the scene with a unique name and a default TransformComponent.
	 *
	 * Creates a new entity, assigns it a TransformComponent, and generates a unique name using the base
	 * tag "GameObject" with an incremented number if needed. The entity is then stored in the scene's
	 * tag-to-entity map.
	 *
	 * @return true Always returns true upon successful creation.
	 */
	bool AddGameObject();

	/**
	 * @brief Adds an existing entity to the scene with a specified tag.
	 *
	 * Inserts the given entity into the tag-to-entity map using the provided tag,
	 * ensuring the tag does not already exist. Logs an error and returns false if
	 * the tag is already in use.
	 *
	 * @param sTag The tag to associate with the entity.
	 * @param entity The entity to add, which must be valid (not entt::null).
	 * @return true if the entity was successfully added; false if the tag already exists.
	 */
	bool AddGameObjectByTag( const std::string& sTag, entt::entity entity );

	/**
	 * @brief Duplicates an existing game object by copying all of its components to a new entity.
	 *
	 * Searches for the given entity in the tag-to-entity map, and if found, creates a new entity
	 * in the registry and copies all of the original entity's components using registered meta functions.
	 * The duplicated entity is given a unique name based on the original tag and added to the map.
	 *
	 * @param entity The entity to duplicate.
	 * @return true if the duplication was successful; false if the original entity was not found in the map.
	 */
	bool DuplicateGameObject( entt::entity entity );

	/**
	 * @brief Deletes a game object and its hierarchy from the scene using the specified tag.
	 *
	 * Locates the entity associated with the given tag and removes it, along with any related child entities,
	 * from the registry using RelationshipUtils. All affected tags are then removed from the tag-to-entity map.
	 *
	 * @param sTag The tag identifying the entity to delete.
	 * @return true if the entity and its relationships were successfully removed; false if the tag was not found.
	 */
	bool DeleteGameObjectByTag( const std::string& sTag );

	/**
	 * @brief Deletes a game object and its hierarchy from the scene using the specified entity ID.
	 *
	 * Searches for the tag associated with the given entity, then removes the entity and any of its
	 * related children using RelationshipUtils. All corresponding tags are erased from the tag-to-entity map.
	 *
	 * @param entity The entity to delete.
	 * @return true if the entity and its relationships were successfully removed; false if the entity was not found.
	 */
	bool DeleteGameObjectById( entt::entity entity );

	virtual bool LoadScene() override;
	virtual bool UnloadScene( bool bSaveScene = true ) override;

	/**
	 * @brief Checks whether a tag name already exists in the scene.
	 *
	 * Determines if the given tag is present in the tag-to-entity map.
	 *
	 * @param sTagName The tag name to check.
	 * @return true if the tag exists; false otherwise.
	 */
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
