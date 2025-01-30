#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <sol/sol.hpp>

#define SCENE_MANAGER() SCION_EDITOR::SceneManager::GetInstance()
#define COMMAND_MANAGER() SCENE_MANAGER().GetCommandManager()
#define TOOL_MANAGER() SCENE_MANAGER().GetToolManager()

namespace SCION_CORE::Events
{
class EventDispatcher;
}

namespace SCION_EDITOR
{
class ToolManager;
class SceneObject;
class CommandManager;

class SceneManager
{
  public:
	static SceneManager& GetInstance();

	bool AddScene( const std::string& sSceneName );
	bool AddScene( const std::string& sSceneName, const std::string& sSceneData );
	bool HasScene( const std::string& sSceneName );

	std::shared_ptr<SCION_EDITOR::SceneObject> GetScene( const std::string& sSceneName );
	std::shared_ptr<SCION_EDITOR::SceneObject> GetCurrentScene();

	std::vector<std::string> GetSceneNames() const;
	ToolManager& GetToolManager();
	CommandManager& GetCommandManager();
	SCION_CORE::Events::EventDispatcher& GetDispatcher();

	void SetTileset( const std::string& sTileset );
	bool LoadCurrentScene();
	bool UnloadCurrentScene();

	bool SaveAllScenes();

	bool CheckTagName( const std::string& sTagName );

	inline const std::map<std::string, std::shared_ptr<SCION_EDITOR::SceneObject>>& GetAllScenes() const
	{
		return m_mapScenes;
	}

	inline void SetCurrentScene( const std::string& sSceneName ) { m_sCurrentScene = sSceneName; }
	inline const std::string& GetCurrentSceneName() const { return m_sCurrentScene; }
	inline const std::string& GetCurrentTileset() const { return m_sCurrentTileset; }

	static void CreateSceneManagerLuaBind( sol::state& lua );

  private:
	std::map<std::string, std::shared_ptr<SCION_EDITOR::SceneObject>> m_mapScenes;
	std::string m_sCurrentScene{ "" };
	std::string m_sCurrentTileset{ "" };

	std::unique_ptr<ToolManager> m_pToolManager{ nullptr };
	std::unique_ptr<CommandManager> m_pCommandManager{ nullptr };

	std::unique_ptr<SCION_CORE::Events::EventDispatcher> m_pSceneDispatcher{ nullptr };

  private:
	SceneManager() = default;
	~SceneManager() = default;
	SceneManager( const SceneManager& ) = delete;
	SceneManager& operator=( const SceneManager& ) = delete;
};
} // namespace SCION_EDITOR

#define ADD_SWE_HANDLER( Event, Func, Handler )                                                                        \
	{                                                                                                                  \
		for ( auto& pDispatcher : TOOL_MANAGER().GetDispatchers() )                                                    \
		{                                                                                                              \
			if ( !pDispatcher )                                                                                        \
				continue;                                                                                              \
			pDispatcher->AddHandler<Event, Func>( Handler );                                                           \
		}                                                                                                              \
	}\
