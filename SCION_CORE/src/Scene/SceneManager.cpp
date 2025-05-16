#include "Core/Scene/SceneManager.h"
#include "Core/Scene/Scene.h"

#include "ScionUtilities/ScionUtilities.h"

namespace SCION_CORE
{

SceneManager::SceneManager()
	: m_mapScenes{}
	, m_sCurrentScene{ "" }
	, m_sCurrentTileset{ "" }
{
}

bool SceneManager::AddScene( const std::string& sSceneName, SCION_CORE::EMapType eType )
{
	if ( m_mapScenes.contains( sSceneName ) )
	{
		SCION_ERROR( "Failed to add new scene object - [{}] already exists.", sSceneName );
		return false;
	}

	auto [ itr, bSuccess ] =
		m_mapScenes.emplace( sSceneName, std::move( std::make_shared<Scene>( sSceneName, eType ) ) );
	return bSuccess;
}

bool SceneManager::HasScene( const std::string& sSceneName )
{
	return m_mapScenes.contains( sSceneName );
}

Scene* SceneManager::GetScene( const std::string& sSceneName )
{
	auto sceneItr = m_mapScenes.find( sSceneName );
	if ( sceneItr == m_mapScenes.end() )
	{
		SCION_ERROR( "Failed to get scene object - [{}] does not exist.", sSceneName );
		return nullptr;
	}

	return sceneItr->second.get();
}

Scene* SceneManager::GetCurrentScene()
{
	if ( m_sCurrentScene.empty() )
		return nullptr;

	auto sceneItr = m_mapScenes.find( m_sCurrentScene );
	if ( sceneItr == m_mapScenes.end() )
	{
		SCION_ERROR( "Failed to get scene object - [{}] does not exist.", m_sCurrentScene );
		return nullptr;
	}

	return sceneItr->second.get();
}

std::vector<std::string> SceneManager::GetSceneNames() const
{
	return SCION_UTIL::GetKeys( m_mapScenes );
}

bool SceneManager::LoadCurrentScene()
{
	if ( auto pCurrentScene = GetCurrentScene() )
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

bool SceneManager::CheckHasScene( const std::string& sSceneName )
{
	return m_mapScenes.contains( sSceneName );
}

bool SceneManager::ChangeSceneName( const std::string& sOldName, const std::string& sNewName )
{
	return SCION_UTIL::KeyChange( m_mapScenes, sOldName, sNewName );
}
} // namespace SCION_CORE
