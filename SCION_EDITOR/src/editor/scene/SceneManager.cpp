#include "SceneManager.h"
#include "SceneObject.h"
#include "ScionUtilities/ScionUtilities.h"
#include "editor/tools/ToolManager.h"
#include "editor/tools/TileTool.h"
#include "editor/tools/gizmos/Gizmo.h"
#include "editor/commands/CommandManager.h"

#include "Logger/Logger.h"

namespace SCION_EDITOR
{
SceneManager& SceneManager::GetInstance()
{
	static SceneManager instance{};
	return instance;
}

bool SceneManager::AddScene( const std::string& sSceneName )
{
	if ( m_mapScenes.contains( sSceneName ) )
	{
		SCION_ERROR( "Failed to add new scene object - [{}] already exists.", sSceneName );
		return false;
	}

	auto [ itr, bSuccess ] =
		m_mapScenes.emplace( sSceneName, std::move( std::make_shared<SCION_EDITOR::SceneObject>( sSceneName ) ) );
	return bSuccess;
}

bool SceneManager::AddScene( const std::string& sSceneName, const std::string& sSceneData )
{
	if ( m_mapScenes.contains( sSceneName ) )
	{
		SCION_ERROR( "Failed to add new scene object - [{}] already exists.", sSceneName );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapScenes.emplace(
		sSceneName, std::move( std::make_shared<SCION_EDITOR::SceneObject>( sSceneName, sSceneData ) ) );
	return bSuccess;
}

bool SceneManager::HasScene( const std::string& sSceneName )
{
	return m_mapScenes.contains( sSceneName );
}

std::shared_ptr<SCION_EDITOR::SceneObject> SceneManager::GetScene( const std::string& sSceneName )
{
	auto sceneItr = m_mapScenes.find( sSceneName );
	if ( sceneItr == m_mapScenes.end() )
	{
		SCION_ERROR( "Failed to get scene object - [{}] does not exist.", sSceneName );
		return nullptr;
	}

	return sceneItr->second;
}

std::shared_ptr<SCION_EDITOR::SceneObject> SceneManager::GetCurrentScene()
{
	if ( m_sCurrentScene.empty() )
		return nullptr;

	auto sceneItr = m_mapScenes.find( m_sCurrentScene );
	if ( sceneItr == m_mapScenes.end() )
	{
		SCION_ERROR( "Failed to get scene object - [{}] does not exist.", m_sCurrentScene );
		return nullptr;
	}

	return sceneItr->second;
}

void SceneManager::AddLayerToCurrentScene( const std::string& sLayerName, bool bVisible )
{
	if ( auto pCurrentScene = GetCurrentScene() )
	{
		pCurrentScene->AddLayer(sLayerName, bVisible);
		return;
	}

	SCION_ERROR( "Failed to add layer. Current scene is nullptr." );
}

std::vector<std::string> SceneManager::GetSceneNames() const
{
	return SCION_UTIL::GetKeys( m_mapScenes );
}

ToolManager& SceneManager::GetToolManager()
{
	if ( !m_pToolManager )
		m_pToolManager = std::make_unique<ToolManager>();

	SCION_ASSERT( m_pToolManager && "Tool manager must be valid" );

	return *m_pToolManager;
}

CommandManager& SceneManager::GetCommandManager()
{
	if ( !m_pCommandManager )
		m_pCommandManager = std::make_unique<CommandManager>();

	SCION_ASSERT( m_pCommandManager && "Command manager must be valid" );

	return *m_pCommandManager;
}

void SceneManager::SetTileset( const std::string& sTileset )
{
	m_sCurrentTileset = sTileset;

	if ( !m_pToolManager )
		return;

	m_pToolManager->SetToolsCurrentTileset( sTileset );
}

bool SceneManager::LoadCurrentScene()
{
	if (auto pCurrentScene = GetCurrentScene())
	{
		return pCurrentScene->LoadScene();
	}

	return false;
}

bool SceneManager::UnloadCurrentScene()
{
	if ( auto pCurrentScene = GetCurrentScene() )
	{
		return pCurrentScene->UnloadScene();
	}

	return false;
}

bool SceneManager::SaveAllScenes()
{
	bool bSuccess{ true };
	for (const auto& [sName, pScene] : m_mapScenes)
	{
		if (!pScene->SaveScene())
		{
			SCION_ERROR( "Failed to save scene [{}]", sName );
			bSuccess = false;
		}
	}

	return bSuccess;
}

} // namespace SCION_EDITOR
