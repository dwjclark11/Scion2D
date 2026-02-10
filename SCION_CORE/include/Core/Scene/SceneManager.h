#pragma once
#include <sol/sol.hpp>

namespace Scion::Core
{

enum class EMapType;
class Scene;
namespace ECS
{
class Registry;
}

struct SceneManagerData
{
	std::string sSceneName{};
	std::string sDefaultMusic{};
	// TODO: Add different stuff
};

class SceneManager
{
  public:
	SceneManager();
	virtual ~SceneManager() {}

	/**
	 * @brief Adds a new scene to the scene manager with the given name and map type.
	 *
	 * Creates and stores a new Scene object if the name is not already in use. Logs an error and returns false
	 * if a scene with the same name already exists.
	 *
	 * @param sSceneName The name of the scene to add.
	 * @param eType The type of the map associated with the scene.
	 * @return true if the scene was successfully added; false if the name already exists.
	 */
	virtual bool AddScene( const std::string& sSceneName, Scion::Core::EMapType eType );
	bool HasScene( const std::string& sSceneName );

	/**
	 * @brief Checks whether a the scene exists and returns a pointer to that scene.
	 *
	 * Determines if the given scene name is present in the sceneName-to-scene map.
	 *
	 * @param sSceneName The scene name to check.
	 * @return a pointer to the scene if exists, nullptr otherwise.
	 */
	Scene* GetScene( const std::string& sSceneName );

	/**
	 * @brief Returns a pointer to the current selected scene.
	 *
	 * @return pointer to the current scene if set, nullptr otherwise.
	 */
	Scene* GetCurrentScene();

	std::vector<std::string> GetSceneNames() const;
	bool LoadCurrentScene();
	bool UnloadCurrentScene();
	bool CheckHasScene( const std::string& sSceneName );
	bool ChangeSceneName( const std::string& sOldName, const std::string& sNewName );

	inline void SetCurrentScene( const std::string& sSceneName ) { m_sCurrentScene = sSceneName; }
	inline const std::string& GetCurrentSceneName() const { return m_sCurrentScene; }

	static void CreateLuaBind( sol::state& lua, ECS::Registry& registry );

  protected:
	std::map<std::string, std::shared_ptr<Scene>> m_mapScenes;
	std::string m_sCurrentScene{};
	std::string m_sCurrentTileset{};
};
} // namespace Scion::Core
