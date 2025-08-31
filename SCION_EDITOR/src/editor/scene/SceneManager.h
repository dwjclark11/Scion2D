#pragma once
#include <sol/sol.hpp>
#include "Core/Scene/SceneManager.h"

#define SCENE_MANAGER() SCION_EDITOR::EditorSceneManager::GetInstance()
#define COMMAND_MANAGER() SCENE_MANAGER().GetCommandManager()
#define TOOL_MANAGER() SCENE_MANAGER().GetToolManager()

namespace SCION_CORE
{
enum class EMapType;
namespace Events
{
class EventDispatcher;
}
} // namespace SCION_CORE

namespace SCION_EDITOR
{
class ToolManager;
class SceneObject;
class CommandManager;

class EditorSceneManager : public SCION_CORE::SceneManager
{
  public:
	static EditorSceneManager& GetInstance();

	virtual bool AddScene( const std::string& sSceneName, SCION_CORE::EMapType eType ) override;
	bool AddSceneObject( const std::string& sSceneName, const std::string& sSceneData );
	bool DeleteScene( const std::string& sSceneName );

	ToolManager& GetToolManager();
	CommandManager& GetCommandManager();
	SCION_CORE::Events::EventDispatcher& GetDispatcher();

	void SetTileset( const std::string& sTileset );

	SceneObject* GetCurrentSceneObject();

	bool SaveAllScenes();

	bool CheckTagName( const std::string& sTagName );

	void UpdateScenes();

	std::string GetSceneFilepath( const std::string& sSceneName );

	inline const std::map<std::string, std::shared_ptr<SCION_CORE::Scene>>& GetAllScenes() const { return m_mapScenes; }

	inline const std::string& GetCurrentTileset() const { return m_sCurrentTileset; }

	static void CreateSceneManagerLuaBind( sol::state& lua );

  private:
	std::unique_ptr<ToolManager> m_pToolManager{ nullptr };
	std::unique_ptr<CommandManager> m_pCommandManager{ nullptr };

	std::unique_ptr<SCION_CORE::Events::EventDispatcher> m_pSceneDispatcher{ nullptr };

  private:
	EditorSceneManager();
	virtual ~EditorSceneManager() = default;
	EditorSceneManager( const EditorSceneManager& ) = delete;
	EditorSceneManager& operator=( const EditorSceneManager& ) = delete;
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
