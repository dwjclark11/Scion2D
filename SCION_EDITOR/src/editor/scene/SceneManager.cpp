#include "SceneManager.h"
#include "SceneObject.h"
#include "ScionUtilities/ScionUtilities.h"
#include "editor/tools/ToolManager.h"
#include "editor/tools/TileTool.h"
#include "editor/tools/gizmos/Gizmo.h"
#include "editor/commands/CommandManager.h"
#include "editor/loaders/ProjectLoader.h"

#include "Core/Events/EventDispatcher.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/CoreUtilities/SaveProject.h"


#include "Logger/Logger.h"

#include <filesystem>

namespace fs = std::filesystem;

using namespace SCION_CORE::ECS;

namespace SCION_EDITOR
{
EditorSceneManager& EditorSceneManager::GetInstance()
{
	static EditorSceneManager instance{};
	return instance;
}

bool EditorSceneManager::AddScene( const std::string& sSceneName, SCION_CORE::EMapType eType )
{
	if ( m_mapScenes.contains( sSceneName ) )
	{
		SCION_ERROR( "Failed to add new scene object - [{}] already exists.", sSceneName );
		return false;
	}

	auto [ itr, bSuccess ] =
		m_mapScenes.emplace( sSceneName, std::move( std::make_shared<SceneObject>( sSceneName, eType ) ) );
	return bSuccess;
}

bool EditorSceneManager::AddSceneObject( const std::string& sSceneName, const std::string& sSceneData )
{
	if ( m_mapScenes.contains( sSceneName ) )
	{
		SCION_ERROR( "Failed to add new scene object - [{}] already exists.", sSceneName );
		return false;
	}

	auto [ itr, bSuccess ] =
		m_mapScenes.emplace( sSceneName, std::move( std::make_shared<SceneObject>( sSceneName, sSceneData ) ) );
	return bSuccess;		
}

bool EditorSceneManager::DeleteScene( const std::string& sSceneName )
{
	auto sceneItr = m_mapScenes.find( sSceneName );
	if (sceneItr == m_mapScenes.end())
	{
		SCION_ERROR( "Failed to delete scene [{}] - Does not exist in the scene manager.", sSceneName );
		return false;
	}

	// Check to see if the scene is loaded. We do not want to delete a loaded scene.
	if (sceneItr->second->IsLoaded())
	{
		SCION_ERROR( "Failed to delete scene [{}] - Loaded scenes cannot be delete. Please unload the scene if you "
					 "want to delete it.",
					 sSceneName );
		return false;
	}

	const std::string& sDataPath{ sceneItr->second->GetSceneDataPath() };
	fs::path dataPath{ sDataPath };

	if ( fs::exists(dataPath.parent_path()) && fs::is_directory( dataPath.parent_path() ) )
	{
		if ( dataPath.parent_path().stem().string() == sSceneName )
		{
			std::error_code ec;
			if (!fs::remove_all(dataPath.parent_path(), ec))
			{
				SCION_ERROR( "Failed to delete scene [{}] and remove files.", sSceneName, ec.message() );
				return false;
			}
		}
	}

	// Recheck if the path exists
	if (fs::exists(dataPath.parent_path()))
	{
		SCION_ERROR( "Failed to delete scene [{}] and remove files.", sSceneName );
		return false;
	}

	if (m_mapScenes.erase(sSceneName) > 0)
	{
		auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<SCION_CORE::SaveProject>>();
		SCION_ASSERT( pSaveProject && "Save Project must exist!" );
		// Save entire project
		ProjectLoader pl{};
		if ( !pl.SaveLoadedProject( *pSaveProject ) )
		{
			SCION_ERROR( "Failed to save project [{}] at file [{}] after deleting scene. Please ensure the scene files have been removed.",
						 pSaveProject->sProjectName,
						 pSaveProject->sProjectFilePath );

			return false;
		}
	}

	return true;
}

ToolManager& EditorSceneManager::GetToolManager()
{
	if ( !m_pToolManager )
		m_pToolManager = std::make_unique<ToolManager>();

	SCION_ASSERT( m_pToolManager && "Tool manager must be valid" );

	return *m_pToolManager;
}

CommandManager& EditorSceneManager::GetCommandManager()
{
	if ( !m_pCommandManager )
		m_pCommandManager = std::make_unique<CommandManager>();

	SCION_ASSERT( m_pCommandManager && "Command manager must be valid" );

	return *m_pCommandManager;
}

SCION_CORE::Events::EventDispatcher& EditorSceneManager::GetDispatcher()
{
	if ( !m_pSceneDispatcher )
		m_pSceneDispatcher = std::make_unique<SCION_CORE::Events::EventDispatcher>();

	SCION_ASSERT( m_pSceneDispatcher && "Event Dispatcher must be valid" );

	return *m_pSceneDispatcher;
}

void EditorSceneManager::SetTileset( const std::string& sTileset )
{
	m_sCurrentTileset = sTileset;

	if ( !m_pToolManager )
		return;

	m_pToolManager->SetToolsCurrentTileset( sTileset );
}


SceneObject* EditorSceneManager::GetCurrentSceneObject()
{
	if ( auto pCurrentScene = dynamic_cast<SceneObject*>( GetCurrentScene() ) )
		return pCurrentScene;

	return nullptr;
}

bool EditorSceneManager::SaveAllScenes()
{
	bool bSuccess{ true };
	for ( const auto& [ sName, pScene ] : m_mapScenes )
	{
		if ( !pScene->SaveScene() )
		{
			SCION_ERROR( "Failed to save scene [{}]", sName );
			bSuccess = false;
		}
	}

	return bSuccess;
}

bool EditorSceneManager::CheckTagName( const std::string& sTagName )
{
	if ( auto pScene = GetCurrentScene() )
	{
		auto pSceneObject = dynamic_cast<SceneObject*>( pScene );
		return pSceneObject->CheckTagName( sTagName );
	}

	return false;
}

EditorSceneManager::EditorSceneManager()
	: SCION_CORE::SceneManager()
{

}

void EditorSceneManager::CreateSceneManagerLuaBind( sol::state& lua )
{
	auto& sceneManager = SCENE_MANAGER();

	lua.new_usertype<EditorSceneManager>(
		"SceneManager", sol::no_constructor, "changeScene", [ & ]( const std::string& sSceneName ) {
			

			auto pCurrentScene = sceneManager.GetCurrentSceneObject();
			if ( !pCurrentScene )
			{
				SCION_ERROR( "Failed to change to scene [{}] - Current scene is invalid.", sSceneName );
				return false;
			}

			if ( pCurrentScene->GetRuntimeName() == sSceneName )
			{
				SCION_ERROR( "Failed to load scene [{}] - Scene has already been loaded.", sSceneName );
				return false;
			}

			auto pScene = sceneManager.GetScene( sSceneName );
			if ( !pScene )
			{
				SCION_ERROR( "Failed to change to scene [{}] - Scene [{}] is invalid.", sSceneName, sSceneName );
				return false;
			}

			pCurrentScene->GetRuntimeRegistry().DestroyEntities<ScriptComponent>();
			if ( !pScene->IsLoaded() )
			{
				pScene->LoadScene();
			}

			// TODO: Check to see if this is valid
			auto pSceneObject = dynamic_cast<SceneObject*>( pScene );
			SCION_ASSERT( pSceneObject );

			pCurrentScene->CopySceneToRuntime( *pSceneObject );
			pScene->UnloadScene();

			return true;
		}, "getCanvas", // Returns the canvas of the current scene or an empty canvas object.
		[ & ] {
			if ( auto pCurrentScene = sceneManager.GetCurrentScene() )
				return pCurrentScene->GetCanvas();

			return SCION_CORE::Canvas{};
		}
	);
}

} // namespace SCION_EDITOR
