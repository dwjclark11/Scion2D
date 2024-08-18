#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

#define SCENE_MANAGER() SCION_EDITOR::SceneManager::GetInstance()

namespace SCION_EDITOR
{
class ToolManager;
class SceneObject;

class SceneManager
{
  private:
	std::map<std::string, std::shared_ptr<SCION_EDITOR::SceneObject>> m_mapScenes;
	std::string m_sCurrentScene{ "" }, m_sCurrentTileset{ "" };

	std::unique_ptr<ToolManager> m_pToolManager{ nullptr };

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
	ToolManager& GetToolManager();
	void SetTileset( const std::string& sTileset );

	inline void SetCurrentScene( const std::string& sSceneName ) { m_sCurrentScene = sSceneName; }
	inline const std::string& GetCurrentSceneName() const { return m_sCurrentScene; }
	inline const std::string& GetCurrentTileset() const { return m_sCurrentTileset; }
};
} // namespace SCION_EDITOR
