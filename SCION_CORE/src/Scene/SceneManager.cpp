#include "Core/Scene/SceneManager.h"
#include "Core/Scene/Scene.h"

#include "ScionUtilities/ScionUtilities.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/Registry.h"
#include "Core/Loaders/TilemapLoader.h"

using namespace Scion::Core::ECS;

namespace Scion::Core
{

SceneManager::SceneManager()
	: m_mapScenes{}
	, m_sCurrentScene{}
	, m_sCurrentTileset{}
{
}

bool SceneManager::AddScene( const std::string& sSceneName, Scion::Core::EMapType eType )
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
	return Scion::Utilities::GetKeys( m_mapScenes );
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
	return Scion::Utilities::KeyChange( m_mapScenes, sOldName, sNewName );
}

void SceneManager::CreateLuaBind( sol::state& lua, ECS::Registry& registry )
{
	lua.new_usertype<SceneManager>(
		"SceneManager",
		sol::no_constructor,
		"setCurrentScene",
		[ &registry ]( const std::string& sSceneName )
		{
			auto* pSceneManagerData = registry.TryGetContext<std::shared_ptr<SceneManagerData>>();
			if (!pSceneManagerData)
			{
				SCION_ERROR( "Scene manager data was not set correctly." );
				return;
			}

			(*pSceneManagerData)->sSceneName = sSceneName;
		},
		"changeScene",
		[ & ]( const std::string& sSceneName ) {
			auto* pSceneManagerData = registry.TryGetContext<std::shared_ptr<SceneManagerData>>();
			if ( !pSceneManagerData )
			{
				SCION_ERROR( "Scene manager data was not set correctly." );
				return false;
			}

			( *pSceneManagerData )->sSceneName = sSceneName;

			sol::optional<sol::table> optSceneData = lua[ sSceneName + "_data" ];
			if ( optSceneData )
			{
				( *pSceneManagerData )->sDefaultMusic = ( *optSceneData )[ "default_music" ].get_or( std::string{} );
			}

			registry.DestroyEntities();

			Scion::Core::Loaders::TilemapLoader tl{};

			tl.LoadTilemapFromLuaTable( registry, lua[ sSceneName + "_tilemap" ] );
			tl.LoadGameObjectsFromLuaTable( registry, lua[ sSceneName + "_objects" ] );

			return true;
		},
		"getCanvas", // Returns the canvas of the current scene or an empty canvas object.
		[ & ] {
			auto* pSceneManagerData = registry.TryGetContext<std::shared_ptr<SceneManagerData>>();
			if ( !pSceneManagerData )
			{
				SCION_ERROR( "Scene manager data was not set correctly." );
				return Scion::Core::Canvas{};
			}

			sol::optional<sol::table> optSceneData = lua[ (*pSceneManagerData)->sSceneName + "_data" ];
			if ( optSceneData )
			{
				if ( sol::optional<sol::table> optCanvas = ( *optSceneData )[ "canvas" ] )
				{
					return Scion::Core::Canvas{ .width = ( *optCanvas )[ "width" ].get_or( 640 ),
											   .height = ( *optCanvas )[ "height" ].get_or( 480 ),
											   .tileWidth = ( *optCanvas )[ "tileWidth" ].get_or( 16 ),
											   .tileHeight = ( *optCanvas )[ "tileHeight" ].get_or( 16 ) };
				}
			}

			return Scion::Core::Canvas{};
		},
		"getDefaultMusic",
		[ & ] {
			auto* pSceneManagerData = registry.TryGetContext<std::shared_ptr<SceneManagerData>>();
			if ( !pSceneManagerData )
			{
				SCION_ERROR( "Scene manager data was not set correctly." );
				return std::string{};
			}

			if ((*pSceneManagerData)->sDefaultMusic.empty())
			{
				sol::optional<sol::table> optSceneData = lua[ ( *pSceneManagerData )->sSceneName + "_data" ];
				if ( optSceneData )
				{
					for (const auto& [key, value] : *optSceneData)
					{
						std::string sKey = key.as<std::string>();
						int x{};
					}
					( *pSceneManagerData )->sDefaultMusic = ( *optSceneData )[ "default_music" ].get_or( std::string{} );
				}
			}

			return ( *pSceneManagerData )->sDefaultMusic;
		},
		"getCurrentSceneName",
		[ & ] {
			auto* pSceneManagerData = registry.TryGetContext<std::shared_ptr<SceneManagerData>>();
			if ( !pSceneManagerData )
			{
				SCION_ERROR( "Scene manager data was not set correctly." );
				return std::string{};
			}

			return (*pSceneManagerData)->sSceneName;
		} );
}

} // namespace Scion::Core
