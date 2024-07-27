#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

#define SCENE_MANAGER() SCION_EDITOR::SceneManager::GetInstance()

namespace SCION_EDITOR
{
class SceneObject;

class SceneManager
{
  private:
	std::map<std::string, std::shared_ptr<SCION_EDITOR::SceneObject>> m_mapScenes;
	std::string m_sCurrentScene{ "" };

  private:
	SceneManager() = default;
	~SceneManager() = default;
	SceneManager( const SceneManager& ) = delete;
	SceneManager& operator=( const SceneManager& ) = delete;

  public:
	static SceneManager& GetInstance();

	bool AddScene( const std::string& sSceneName );
	std::shared_ptr<SCION_EDITOR::SceneObject> GetScene( const std::string& sSceneName );
	std::shared_ptr<SCION_EDITOR::SceneObject> GetCurrentScene();
	std::vector<std::string> GetSceneNames() const;

	inline void SetCurrentScene( const std::string& sSceneName ) { m_sCurrentScene = sSceneName; }
	inline const std::string& GetCurrentSceneName() const { return m_sCurrentScene; }
};
} // namespace SCION_EDITOR