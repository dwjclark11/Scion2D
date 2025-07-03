#include "SceneObject.h"

#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MetaUtilities.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Loaders/TilemapLoader.h"
#include "Core/Events/EventDispatcher.h"

#include "Core/CoreUtilities/SaveProject.h"
#include "editor/events/EditorEventTypes.h"

#include "ScionUtilities/ScionUtilities.h"
#include "ScionFilesystem/Serializers/JSONSerializer.h"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <fmt/format.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

using namespace SCION_FILESYSTEM;
using namespace SCION_CORE::ECS;
using namespace SCION_CORE::Loaders;
using namespace entt::literals;

namespace SCION_EDITOR
{

SceneObject::SceneObject( const std::string& sceneName, SCION_CORE::EMapType eType )
	: Scene( sceneName, eType )
	, m_RuntimeRegistry{}
	, m_pRuntimeData{ nullptr }
{
	ADD_EVENT_HANDLER( SCION_EDITOR::Events::NameChangeEvent, &SceneObject::OnEntityNameChanges, *this );
}

SceneObject::SceneObject( const std::string& sceneName, const std::string& sceneData )
	: m_RuntimeRegistry{}
{
	m_sSceneName = sceneName;
	m_sSceneDataPath = sceneData;

	// We need to load the scene data from the json file!
	if ( !LoadSceneData() )
	{
		SCION_ERROR( "Failed to load scene data" );
		return;
	}

	// Verify that the tilemap and objectmap files exist.
	if ( !fs::exists( fs::path{ m_sTilemapPath } ) )
	{
		SCION_WARN( "Tilemap file [{}] does not exist.", m_sTilemapPath );
	}

	if ( !fs::exists( fs::path{ m_sObjectPath } ) )
	{
		SCION_WARN( "Object file [{}] does not exist.", m_sObjectPath );
	}

	ADD_EVENT_HANDLER( SCION_EDITOR::Events::NameChangeEvent, &SceneObject::OnEntityNameChanges, *this );
}

void SceneObject::CopySceneToRuntime()
{
	if ( !m_pRuntimeData )
	{
		m_pRuntimeData = std::make_unique<SceneRuntimeData>();
	}

	// Setup runtime data
	m_pRuntimeData->canvas = m_Canvas;
	m_pRuntimeData->sDefaultMusic = m_sDefaultMusic;
	m_pRuntimeData->sSceneName = m_sSceneName;

	auto& registryToCopy = m_Registry.GetRegistry();

	for ( auto entityToCopy : registryToCopy.view<entt::entity>( entt::exclude<ScriptComponent, UneditableComponent> ) )
	{
		entt::entity newEntity = m_RuntimeRegistry.CreateEntity();

		// Copy the components of the entity to the new entity
		for ( auto&& [ id, storage ] : registryToCopy.storage() )
		{
			if ( !storage.contains( entityToCopy ) )
				continue;

			SCION_CORE::Utils::InvokeMetaFunction(
				id, "copy_component"_hs, Entity{ m_Registry, entityToCopy }, Entity{ m_RuntimeRegistry, newEntity } );
		}
	}

	if ( m_bUsePlayerStart )
	{
		m_PlayerStart.CreatePlayer( m_RuntimeRegistry );
	}
}

void SceneObject::CopySceneToRuntime( SceneObject& sceneToCopy )
{
	if ( !m_pRuntimeData )
	{
		m_pRuntimeData = std::make_unique<SceneRuntimeData>();
	}

	// Setup runtime data
	m_pRuntimeData->canvas = sceneToCopy.GetCanvas();
	m_pRuntimeData->sDefaultMusic = sceneToCopy.GetDefaultMusicName();
	m_pRuntimeData->sSceneName = sceneToCopy.GetName();

	auto& registry = sceneToCopy.GetRegistry();
	auto& registryToCopy = registry.GetRegistry();

	for ( auto entityToCopy : registryToCopy.view<entt::entity>( entt::exclude<ScriptComponent, UneditableComponent> ) )
	{
		entt::entity newEntity = m_RuntimeRegistry.CreateEntity();

		// Copy the components of the entity to the new entity
		for ( auto&& [ id, storage ] : registryToCopy.storage() )
		{
			if ( !storage.contains( entityToCopy ) )
				continue;

			SCION_CORE::Utils::InvokeMetaFunction(
				id, "copy_component"_hs, Entity{ registry, entityToCopy }, Entity{ m_RuntimeRegistry, newEntity } );
		}
	}

	// We want to copy the player start from the new scene.
	if ( sceneToCopy.IsPlayerStartEnabled() )
	{
		sceneToCopy.CopyPlayerStartToRuntimeRegistry( m_RuntimeRegistry );
	}
}

void SceneObject::CopyPlayerStartToRuntimeRegistry( SCION_CORE::ECS::Registry& runtimeRegistry )
{
	if ( !m_bUsePlayerStart )
	{
		SCION_ERROR( "Failed to copy player to runtime. Not enabled." );
		return;
	}

	m_PlayerStart.CreatePlayer( runtimeRegistry );
}

void SceneObject::ClearRuntimeScene()
{
	m_RuntimeRegistry.ClearRegistry();
	m_pRuntimeData.reset();
}

void SceneObject::AddNewLayer()
{
	bool bCheckLayer{ true };
	size_t currentLayer{ 0 };
	while ( bCheckLayer )
	{
		auto hasLayerItr = std::ranges::find_if( m_LayerParams, [ & ]( const auto& layerParam ) {
			return layerParam.sLayerName == fmt::format( "NewLayer_{}", currentLayer );
		} );

		if ( hasLayerItr != m_LayerParams.end() )
		{
			++currentLayer;
		}
		else
		{
			bCheckLayer = false;
		}
	}

	m_LayerParams.emplace_back(
		SCION_UTIL::SpriteLayerParams{ .sLayerName = fmt::format( "NewLayer_{}", currentLayer ) } );
}

bool SceneObject::AddGameObject()
{
	Entity newObject{ m_Registry, "", "" };
	newObject.AddComponent<TransformComponent>();
	std::string sTag{ "GameObject" };

	auto objItr = m_mapTagToEntity.find( sTag );
	if ( objItr != m_mapTagToEntity.end() )
	{
		size_t objIndex{ 1 };
		sTag = "GameObject1";
		objItr = m_mapTagToEntity.find( sTag );
		while ( objItr != m_mapTagToEntity.end() )
		{
			++objIndex;
			sTag = fmt::format( "GameObject{}", objIndex );
			objItr = m_mapTagToEntity.find( sTag );
		}
	}

	newObject.ChangeName( sTag );
	m_mapTagToEntity.emplace( sTag, newObject.GetEntity() );

	return true;
}

bool SceneObject::AddGameObjectByTag( const std::string& sTag, entt::entity entity )
{
	SCION_ASSERT( entity != entt::null && "The entity passed in must be valid." );

	if ( m_mapTagToEntity.contains( sTag ) )
	{
		SCION_ERROR( "Failed to add entity with tag [{}] - Already Exists!", sTag );
		return false;
	}

	m_mapTagToEntity.emplace( sTag, entity );

	return true;
}

bool SceneObject::DuplicateGameObject( entt::entity entity )
{
	auto objItr = m_mapTagToEntity.begin();
	for ( ; objItr != m_mapTagToEntity.end(); ++objItr )
	{
		if ( objItr->second == entity )
			break;
	}

	if ( objItr == m_mapTagToEntity.end() )
	{
		SCION_ERROR( "Failed to duplicate game object with id [{}]. Does not exist or was not mapped correctly.",
					 static_cast<uint32_t>( entity ) );
		return false;
	}

	// Create the new entity in the registry
	auto& registry = m_Registry.GetRegistry();
	auto newEntity = registry.create();

	// Copy the components of the entity to the new entity
	for ( auto&& [ id, storage ] : registry.storage() )
	{
		if ( !storage.contains( entity ) )
			continue;

		SCION_CORE::Utils::InvokeMetaFunction(
			id, "copy_component"_hs, Entity{ m_Registry, entity }, Entity{ m_Registry, newEntity } );
	}

	// Now we need to set the tag for the entity
	size_t tagNum{ 1 };

	while ( CheckTagName( fmt::format( "{}_{}", objItr->first, tagNum ) ) )
	{
		++tagNum;
	}

	Entity newEnt{ m_Registry, newEntity };
	newEnt.ChangeName( fmt::format( "{}_{}", objItr->first, tagNum ) );

	m_mapTagToEntity.emplace( newEnt.GetName(), newEntity );

	return true;
}

bool SceneObject::DeleteGameObjectByTag( const std::string& sTag )
{
	auto objItr = m_mapTagToEntity.find( sTag );
	if ( objItr == m_mapTagToEntity.end() )
	{
		SCION_ERROR( "Failed to delete game object with tag [{}]. Does not exist or was not mapped correctly.", sTag );
		return false;
	}

	std::vector<std::string> removedEntities;
	Entity ent{ m_Registry, objItr->second };

	RelationshipUtils::RemoveAndDelete( ent, removedEntities );

	for ( const auto& sTag : removedEntities )
	{
		m_mapTagToEntity.erase( sTag );
	}

	return true;
}

bool SceneObject::DeleteGameObjectById( entt::entity entity )
{
	auto objItr = m_mapTagToEntity.begin();
	for ( ; objItr != m_mapTagToEntity.end(); ++objItr )
	{
		if ( objItr->second == entity )
			break;
	}

	if ( objItr == m_mapTagToEntity.end() )
	{
		SCION_ERROR( "Failed to delete game object with id [{}]. Does not exist or was not mapped correctly.",
					 static_cast<uint32_t>( entity ) );
		return false;
	}

	std::vector<std::string> removedEntities;
	Entity ent{ m_Registry, objItr->second };

	RelationshipUtils::RemoveAndDelete( ent, removedEntities );

	for ( const auto& sTag : removedEntities )
	{
		m_mapTagToEntity.erase( sTag );
	}

	return true;
}

bool SceneObject::LoadScene()
{
	bool bSceneLoaded = Scene::LoadScene();

	// Map the entities
	auto view = m_Registry.GetRegistry().view<entt::entity>( entt::exclude<TileComponent> );
	for ( auto entity : view )
	{
		Entity ent{ m_Registry, entity };
		AddGameObjectByTag( ent.GetName(), entity );
	}

	return bSceneLoaded;
}

bool SceneObject::UnloadScene( bool bSaveScene )
{
	bool bSuccess = Scene::UnloadScene( bSaveScene );
	m_mapTagToEntity.clear();
	return bSuccess;
}

bool SceneObject::CheckTagName( const std::string& sTagName )
{
	return m_mapTagToEntity.contains( sTagName );
}

void SceneObject::OnEntityNameChanges( SCION_EDITOR::Events::NameChangeEvent& nameChange )
{
	if ( nameChange.sNewName.empty() || nameChange.sOldName.empty() || !nameChange.pEntity )
		return;

	auto objItr = m_mapTagToEntity.find( nameChange.sOldName );
	if ( objItr == m_mapTagToEntity.end() )
		return;

	// If the map already contains that name, don't change the name
	if ( m_mapTagToEntity.contains( nameChange.sNewName ) )
	{
		nameChange.pEntity->ChangeName( nameChange.sOldName );
		SCION_ERROR( "Failed to change entity name. [{}] already exists.", nameChange.sNewName );
		return;
	}

	if ( nameChange.pEntity->GetEntity() != objItr->second )
		return;

	if ( !SCION_UTIL::KeyChange( m_mapTagToEntity, nameChange.sOldName, nameChange.sNewName ) )
	{
		SCION_ERROR( "Failed to change entity name." );
		return;
	}
}

} // namespace SCION_EDITOR
