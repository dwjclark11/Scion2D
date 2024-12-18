#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

#define SCENE_MANAGER() SCION_EDITOR::SceneManager::GetInstance()
#define COMMAND_MANAGER() SCENE_MANAGER().GetCommandManager()
#define TOOL_MANAGER() SCENE_MANAGER().GetToolManager()

namespace SCION_EDITOR
{
class ToolManager;
class SceneObject;
class CommandManager;

class SceneManager
{
  private:
	std::map<std::string, std::shared_ptr<SCION_EDITOR::SceneObject>> m_mapScenes;
	std::string m_sCurrentScene{ "" };
	std::string m_sCurrentTileset{ "" };

	std::unique_ptr<ToolManager> m_pToolManager{ nullptr };
	std::unique_ptr<CommandManager> m_pCommandManager{ nullptr };

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
	CommandManager& GetCommandManager();

	void SetTileset( const std::string& sTileset );

	inline void SetCurrentScene( const std::string& sSceneName ) { m_sCurrentScene = sSceneName; }
	inline const std::string& GetCurrentSceneName() const { return m_sCurrentScene; }
	inline const std::string& GetCurrentTileset() const { return m_sCurrentTileset; }
};
} // namespace SCION_EDITOR
