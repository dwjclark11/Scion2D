#include "SceneObject.h"

#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MetaUtilities.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Loaders/TilemapLoader.h"
#include "Core/Events/EventDispatcher.h"

#include "editor/utilities/SaveProject.h"
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
SceneObject::SceneObject( const std::string& sceneName )
	: m_Registry{}
	, m_RuntimeRegistry{}
	, m_sSceneName{ sceneName }
	, m_sRuntimeSceneName{ "" }
	, m_sTilemapPath{ "" }
	, m_sObjectPath{ "" }
	, m_sSceneDataPath{ "" }
	, m_Canvas{}
	, m_CurrentLayer{ 0 }
	, m_bSceneLoaded{ false }
{
	auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<SaveProject>>();
	SCION_ASSERT( pSaveProject && "SaveProject must exists here!" );

	std::string sScenePath = fmt::format( "{}content{}assets{}scenes{}{}",
										  pSaveProject->sProjectPath,
										  PATH_SEPARATOR,
										  PATH_SEPARATOR,
										  PATH_SEPARATOR,
										  m_sSceneName );

	if ( fs::exists( sScenePath ) )
	{
		SCION_ERROR( "SCENE ALREADY EXISTS!" );
	}

	std::error_code ec;
	if ( !fs::create_directory( fs::path{ sScenePath }, ec ) )
	{
		SCION_ERROR( "Failed to create scene directory [{}] - Error: ", ec.message() );
	}

	m_sTilemapPath = sScenePath + PATH_SEPARATOR + m_sSceneName + "_tilemap.json";
	m_sObjectPath = sScenePath + PATH_SEPARATOR + m_sSceneName + "_objects.json";
	m_sSceneDataPath = sScenePath + PATH_SEPARATOR + m_sSceneName + "_scene_data.json";

	// Create the files
	std::fstream tilemap{};
	tilemap.open( m_sTilemapPath, std::ios::out | std::ios::trunc );
	SCION_ASSERT( tilemap.is_open() && "File should have been created and opened." );
	tilemap.close();

	std::fstream objectmap{};
	objectmap.open( m_sObjectPath, std::ios::out | std::ios::trunc );
	SCION_ASSERT( objectmap.is_open() && "File should have been created and opened." );
	objectmap.close();

	std::fstream sceneData{};
	sceneData.open( m_sSceneDataPath, std::ios::out | std::ios::trunc );
	SCION_ASSERT( sceneData.is_open() && "File should have been created and opened." );
	sceneData.close();

	ADD_EVENT_HANDLER( SCION_EDITOR::Events::NameChangeEvent, &SceneObject::OnEntityNameChanges, *this );
}

SceneObject::SceneObject( const std::string& sceneName, const std::string& sceneData )
	: m_Registry{}
	, m_RuntimeRegistry{}
	, m_sSceneName{ sceneName }
	, m_sTilemapPath{}
	, m_sObjectPath{}
	, m_sSceneDataPath{ sceneData }
	, m_Canvas{}
	, m_CurrentLayer{ 0 }
	, m_bSceneLoaded{ false }
{
	// We need to load the scene data from the json file!
	if ( !LoadSceneData() )
	{
		SCION_ERROR( "Failed to load scene data" );
		return;
	}

	// Verify that the tilemap and objectmap files exist.
	if ( !fs::exists( fs::path{ m_sTilemapPath } ) )
	{
		// Log Some error??
	}

	if ( !fs::exists( fs::path{ m_sObjectPath } ) )
	{
		// Log Some error??
	}

	ADD_EVENT_HANDLER( SCION_EDITOR::Events::NameChangeEvent, &SceneObject::OnEntityNameChanges, *this );
}

void SceneObject::CopySceneToRuntime()
{
	m_sRuntimeSceneName = m_sSceneName;

	auto& registryToCopy = m_Registry.GetRegistry();

	for ( auto entityToCopy : registryToCopy.view<entt::entity>( entt::exclude<ScriptComponent> ) )
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
}

void SceneObject::CopySceneToRuntime( SceneObject& sceneToCopy )
{
	m_sRuntimeSceneName = sceneToCopy.GetName();

	auto& registry = sceneToCopy.GetRegistry();
	auto& registryToCopy = registry.GetRegistry();

	for ( auto entityToCopy : registryToCopy.view<entt::entity>( entt::exclude<ScriptComponent> ) )
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
}

void SceneObject::ClearRuntimeScene()
{
	m_RuntimeRegistry.ClearRegistry();
	m_sRuntimeSceneName.clear();
}

void SceneObject::AddNewLayer()
{
	m_LayerParams.emplace_back(
		SCION_UTIL::SpriteLayerParams{ .sLayerName = fmt::format( "NewLayer_{}", m_CurrentLayer++ ) } );
}

void SceneObject::AddLayer( const std::string& sLayerName, bool bVisible )
{
	if ( !CheckLayerName( sLayerName ) )
	{
		// SCION_ERROR( "Failed to add layer [{}] - Already exists.", sLayerName );
		return;
	}

	m_LayerParams.emplace_back( SCION_UTIL::SpriteLayerParams{ .sLayerName = sLayerName, .bVisible = bVisible } );
}

bool SceneObject::CheckLayerName( const std::string& sLayerName )
{
	return SCION_UTIL::CheckContainsValue( m_LayerParams, [ & ]( SCION_UTIL::SpriteLayerParams& spriteLayer ) {
		return spriteLayer.sLayerName == sLayerName;
	} );
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
	if ( m_bSceneLoaded )
	{
		SCION_ERROR( "Scene [{}] has already been loaded", m_sSceneName );
		return false;
	}

	if ( !LoadSceneData() )
	{
		SCION_ERROR( "Failed to load scene data" );
		return false;
	}

	// Try to load the tilemap and object maps
	auto pTilemapLoader = std::make_unique<TilemapLoader>();
	if ( !pTilemapLoader->LoadTilemap( m_Registry, m_sTilemapPath, true ) )
	{
	}

	// Load scene game objects
	if ( !pTilemapLoader->LoadGameObjects( m_Registry, m_sObjectPath, true ) )
	{
	}

	// Map the entities
	auto view = m_Registry.GetRegistry().view<entt::entity>( entt::exclude<TileComponent> );
	for ( auto entity : view )
	{
		Entity ent{ m_Registry, entity };
		AddGameObjectByTag( ent.GetName(), entity );
	}

	m_bSceneLoaded = true;
	return true;
}

bool SceneObject::UnloadScene( bool bSaveScene )
{
	if ( !m_bSceneLoaded )
	{
		SCION_ERROR( "Scene [{}] has not been loaded", m_sSceneName );
		return false;
	}

	if ( bSaveScene && !SaveSceneData() )
	{
		SCION_ERROR( "Failed to unload scene data" );
		return false;
	}

	// Remove all objects in registry
	m_Registry.ClearRegistry();
	m_mapTagToEntity.clear();
	m_bSceneLoaded = false;
	return false;
}

bool SceneObject::SaveScene()
{
	return SaveSceneData();
}

bool SceneObject::CheckTagName( const std::string& sTagName )
{
	return m_mapTagToEntity.contains( sTagName );
}

bool SceneObject::LoadSceneData()
{
	std::error_code ec;
	if ( !fs::exists( m_sSceneDataPath, ec ) )
	{
		SCION_ERROR( "Failed to load scene data. Error: {}", ec.message() );
		return false;
	}

	std::ifstream sceneDataFile;
	sceneDataFile.open( m_sSceneDataPath );

	if ( !sceneDataFile.is_open() )
	{
		SCION_ERROR( "Failed to open tilemap file [{}]", m_sSceneDataPath );
		return false;
	}

	// The sceneData file could be empty if just created
	if ( sceneDataFile.peek() == std::ifstream::traits_type::eof() )
	{
		// If the sceneData is an empty file, return true. must not have made any changes yet.
		return true;
	}

	std::stringstream ss;
	ss << sceneDataFile.rdbuf();
	std::string contents = ss.str();
	rapidjson::StringStream jsonStr{ contents.c_str() };

	rapidjson::Document doc;
	doc.ParseStream( jsonStr );

	if ( doc.HasParseError() || !doc.IsObject() )
	{
		SCION_ERROR( "Failed to load tilemap: File: [{}] is not valid JSON. - {} - {}",
					 m_sSceneDataPath,
					 rapidjson::GetParseError_En( doc.GetParseError() ),
					 doc.GetErrorOffset() );
		return false;
	}

	SCION_ASSERT( doc.HasMember( "scene_data" ) && "scene_data member is necessary." );

	const rapidjson::Value& sceneData = doc[ "scene_data" ];

	auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<SaveProject>>();
	SCION_ASSERT( pSaveProject && "SaveProject must exists here!" );

	std::string sScenePath = fmt::format( "{}content{}", pSaveProject->sProjectPath, PATH_SEPARATOR );

	if ( m_sTilemapPath.empty() )
	{
		m_sTilemapPath = sScenePath + sceneData[ "tilemapPath" ].GetString();
	}

	if ( m_sObjectPath.empty() )
	{
		m_sObjectPath = sScenePath + sceneData[ "objectmapPath" ].GetString();
	}

	if ( sceneData.HasMember( "canvas" ) )
	{
		const rapidjson::Value& canvas = sceneData[ "canvas" ];

		m_Canvas.width = canvas[ "width" ].GetInt();
		m_Canvas.height = canvas[ "height" ].GetInt();
		m_Canvas.tileWidth = canvas[ "tileWidth" ].GetInt();
		m_Canvas.tileHeight = canvas[ "tileHeight" ].GetInt();
	}

	SCION_ASSERT( sceneData.HasMember( "sprite_layers" ) && "Sprite layers must be a part of scene data" );
	const rapidjson::Value& spriteLayers = sceneData[ "sprite_layers" ];
	for ( const auto& layer : spriteLayers.GetArray() )
	{
		std::string sLayerName = layer[ "layerName" ].GetString();
		bool bVisible = layer[ "bVisible" ].GetBool();

		AddLayer( sLayerName, bVisible );
	}

	return true;
}

bool SceneObject::SaveSceneData()
{
	/*
	 * Scenes that have not been loaded do not need to be re-saved. They would have been
	 * saved when unloading the scene previously. Only save loaded scenes.
	 */
	if ( !m_bSceneLoaded )
	{
		return true;
	}

	// Check to see if the scene data exists
	fs::path tilemapPath{ m_sSceneDataPath };
	if ( !fs::exists( tilemapPath ) )
	{
		SCION_ERROR( "Failed to save scene data - Filepath does not exist [{}]", m_sSceneDataPath );
		return false;
	}

	std::unique_ptr<JSONSerializer> pSerializer{ nullptr };

	try
	{
		pSerializer = std::make_unique<JSONSerializer>( m_sSceneDataPath );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to save scene data [{}] - [{}]", m_sSceneDataPath, ex.what() );
		return false;
	}

	pSerializer->StartDocument();
	pSerializer->StartNewObject( "scene_data" );

	std::string sTilemapPath = m_sTilemapPath.substr( m_sTilemapPath.find( ASSETS ) );
	std::string sObjectPath = m_sObjectPath.substr( m_sObjectPath.find( ASSETS ) );

	pSerializer->AddKeyValuePair( "name", m_sSceneName )
		.AddKeyValuePair( "tilemapPath", sTilemapPath )
		.AddKeyValuePair( "objectmapPath", sObjectPath )
		.StartNewObject( "canvas" )
		.AddKeyValuePair( "width", m_Canvas.width )
		.AddKeyValuePair( "height", m_Canvas.height )
		.AddKeyValuePair( "tileWidth", m_Canvas.tileWidth )
		.AddKeyValuePair( "tileHeight", m_Canvas.tileHeight )
		.EndObject() // Canvas
		.StartNewArray( "sprite_layers" );

	for ( const auto& layer : m_LayerParams )
	{
		pSerializer->StartNewObject()
			.AddKeyValuePair( "layerName", layer.sLayerName )
			.AddKeyValuePair( "bVisible", layer.bVisible )
			.EndObject();
	}

	pSerializer->EndArray();  // Sprite Layers
	pSerializer->EndObject(); // Scene  data

	bool bSuccess{ true };
	if ( !pSerializer->EndDocument() )
	{
		bSuccess = false;
	}

	// Try to Save the tilemap
	auto pTilemapLoader = std::make_unique<TilemapLoader>();
	if ( !pTilemapLoader->SaveTilemap( m_Registry, m_sTilemapPath, true ) )
	{
		bSuccess = false;
	}

	// Try to Save scene game objects
	if ( !pTilemapLoader->SaveGameObjects( m_Registry, m_sObjectPath, true ) )
	{
		bSuccess = false;
	}

	return bSuccess;
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
