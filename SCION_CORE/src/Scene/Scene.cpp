#include "Core/Scene/Scene.h"
#include "Core/Loaders/TilemapLoader.h"

#include "ScionUtilities/ScionUtilities.h"
#include "ScionFilesystem/Serializers/JSONSerializer.h"

#include "Core/CoreUtilities/ProjectInfo.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MetaUtilities.h"
#include "Core/ECS/MainRegistry.h"

#include "Core/Character/Character.h"
#include "Core/CoreUtilities/Prefab.h"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <fmt/format.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

using namespace Scion::Filesystem;
using namespace Scion::Core::Loaders;

namespace Scion::Core
{
Scene::Scene()
	: m_Registry{}
	, m_sSceneName{}
	, m_sTilemapPath{}
	, m_sObjectPath{}
	, m_sSceneDataPath{}
	, m_sDefaultMusic{}
	, m_bSceneLoaded{ false }
	, m_bUsePlayerStart{ false }
	, m_Canvas{}
	, m_eMapType{ EMapType::Grid }
	, m_PlayerStart{ m_Registry, *this }
{
	// Empty Scene
}

Scene::Scene( const std::string& sceneName, EMapType eType )
	: m_Registry{}
	, m_sSceneName{ sceneName }
	, m_sTilemapPath{}
	, m_sObjectPath{}
	, m_sSceneDataPath{}
	, m_sDefaultMusic{}
	, m_bSceneLoaded{ false }
	, m_bUsePlayerStart{ false }
	, m_Canvas{}
	, m_eMapType{ eType }
	, m_PlayerStart{ m_Registry, *this }
{
	auto& pProjectInfo = MAIN_REGISTRY().GetContext<Scion::Core::ProjectInfoPtr>();
	auto optScenesPath = pProjectInfo->TryGetFolderPath( Scion::Core::EProjectFolderType::Scenes );

	SCION_ASSERT( optScenesPath && "Scenes folder path not set correctly." );

	fs::path scenePath = *optScenesPath /= m_sSceneName;

	if ( fs::exists( scenePath ) )
	{
		SCION_ERROR( "SCENE ALREADY EXISTS!" );
	}

	std::error_code ec;
	if ( !fs::create_directory( scenePath, ec ) )
	{
		SCION_ERROR( "Failed to create scene directory [{}] - Error: ", ec.message() );
	}

	auto tilemapPath = scenePath / fs::path{ m_sSceneName + "_tilemap.json" };
	m_sTilemapPath = tilemapPath.string();
	auto objectPath = scenePath / fs::path{ m_sSceneName + "_objects.json" };
	m_sObjectPath = objectPath.string();
	auto sceneDataPath = scenePath / fs::path{ m_sSceneName + "_scene_data.json" };
	m_sSceneDataPath = sceneDataPath.string();

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

	SaveSceneData( true );
}

bool Scene::LoadScene()
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

	m_bSceneLoaded = true;
	SCION_LOG( "Loaded Scene: {}", m_sSceneName );
	return true;
}

bool Scene::UnloadScene( bool bSaveScene )
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
	m_PlayerStart.Unload();
	m_Registry.ClearRegistry();
	m_bSceneLoaded = false;

	return true;
}

int Scene::AddLayer( const std::string& sLayerName, bool bVisible )
{
	if ( !CheckLayerName( sLayerName ) )
	{
		// SCION_ERROR( "Failed to add layer [{}] - Already exists.", sLayerName );
		return static_cast<int>( m_LayerParams.size() );
	}

	auto& spriteLayer =
		m_LayerParams.emplace_back( Scion::Utilities::SpriteLayerParams{ .sLayerName = sLayerName, .bVisible = bVisible } );
	spriteLayer.layer = m_LayerParams.size() - 1;

	return static_cast<int>( m_LayerParams.size() );
}

int Scene::AddLayer( const Scion::Utilities::SpriteLayerParams& layerParam )
{
	auto layerItr =
		std::ranges::find_if( m_LayerParams, [ &layerParam ]( const auto& lp ) { return lp == layerParam; } );

	SCION_ASSERT( layerItr == m_LayerParams.end() && "Layer already exists!" );

	if (layerItr != m_LayerParams.end())
	{
		SCION_ERROR( "The layer [{]] already exists", layerParam.sLayerName );
		return -1;
	}

	m_LayerParams.push_back( layerParam );

	std::ranges::sort( m_LayerParams, []( const auto& a, const auto& b ) { return a.layer < b.layer; } );

	return 1;
}

bool Scene::CheckLayerName( const std::string& sLayerName )
{
	return Scion::Utilities::CheckContainsValue( m_LayerParams, [ & ]( Scion::Utilities::SpriteLayerParams& spriteLayer ) {
		return spriteLayer.sLayerName == sLayerName;
	} );
}

bool Scene::LoadSceneData()
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

	auto& pProjectInfo = MAIN_REGISTRY().GetContext<ProjectInfoPtr>();
	auto optScenesPath = pProjectInfo->TryGetFolderPath( EProjectFolderType::Scenes );
	SCION_ASSERT( optScenesPath && "Scenes folder path must exist." );

	if ( m_sTilemapPath.empty() )
	{
		const std::string sRelativeTilemap = sceneData[ "tilemapPath" ].GetString();
		fs::path tilemapPath = *optScenesPath / sRelativeTilemap;
		if ( !fs::exists( tilemapPath ) )
		{
			SCION_ERROR( "Failed to set tilemap path: [{}] does not exist.", tilemapPath.string() );
			return false;
		}

		m_sTilemapPath = tilemapPath.string();
	}

	if ( m_sObjectPath.empty() )
	{
		const std::string sRelativeObjectPath = sceneData[ "objectmapPath" ].GetString();
		fs::path objectPath = *optScenesPath / sRelativeObjectPath;
		if ( !fs::exists( objectPath ) )
		{
			SCION_ERROR( "Failed to set tilemap path: [{}] does not exist.", objectPath.string() );
			return false;
		}
		m_sObjectPath = objectPath.string();
	}

	if ( sceneData.HasMember( "canvas" ) )
	{
		const rapidjson::Value& canvas = sceneData[ "canvas" ];

		m_Canvas.width = canvas[ "width" ].GetInt();
		m_Canvas.height = canvas[ "height" ].GetInt();
		m_Canvas.tileWidth = canvas[ "tileWidth" ].GetInt();
		m_Canvas.tileHeight = canvas[ "tileHeight" ].GetInt();
	}

	if ( sceneData.HasMember( "mapType" ) )
	{
		std::string sMapType = sceneData[ "mapType" ].GetString();
		if ( sMapType == "grid" )
		{
			m_eMapType = Scion::Core::EMapType::Grid;
		}
		else if ( sMapType == "iso" )
		{
			m_eMapType = Scion::Core::EMapType::IsoGrid;
			SetCanvasOffset();
		}
	}

	if ( sceneData.HasMember( "playerStart" ) )
	{
		const rapidjson::Value& playerStart = sceneData[ "playerStart" ];
		if ( playerStart.HasMember( "enabled" ) )
		{
			m_bUsePlayerStart = playerStart[ "enabled" ].GetBool();
		}
		else
		{
			m_bUsePlayerStart = false;
		}

		std::string sPlayerStartPrefab = sceneData[ "playerStart" ][ "character" ].GetString();
		if ( sPlayerStartPrefab != "default" )
		{
			m_PlayerStart.Load( sPlayerStartPrefab );
		}

		if ( m_bUsePlayerStart && !m_PlayerStart.IsPlayerStartCreated() )
		{
			m_PlayerStart.LoadVisualEntity();
		}

		// Do not set the position if we are not using the player start.
		if ( m_bUsePlayerStart )
		{
			m_PlayerStart.SetPosition( glm::vec2{ sceneData[ "playerStart" ][ "position" ][ "x" ].GetFloat(),
												  sceneData[ "playerStart" ][ "position" ][ "y" ].GetFloat() } );
		}
	}

	if ( sceneData.HasMember( "defaultMusic" ) )
	{
		m_sDefaultMusic = sceneData[ "defaultMusic" ].GetString();
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

bool Scene::SaveSceneData( bool bOverride )
{
	/*
	 * Scenes that have not been loaded do not need to be re-saved. They would have been
	 * saved when unloading the scene previously. Only save loaded scenes.
	 */
	if ( !m_bSceneLoaded && !bOverride )
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

	std::string sTilemapPath = m_sTilemapPath.substr( m_sTilemapPath.find( m_sSceneName ) );
	std::string sObjectPath = m_sObjectPath.substr( m_sObjectPath.find( m_sSceneName ) );

	glm::vec2 playerStartPosition = m_bUsePlayerStart ? m_PlayerStart.GetPosition() : glm::vec2{ 0.f };

	pSerializer->AddKeyValuePair( "name", m_sSceneName )
		.AddKeyValuePair( "tilemapPath", sTilemapPath )
		.AddKeyValuePair( "objectmapPath", sObjectPath )
		.AddKeyValuePair( "defaultMusic", m_sDefaultMusic )
		.StartNewObject( "canvas" )
		.AddKeyValuePair( "width", m_Canvas.width )
		.AddKeyValuePair( "height", m_Canvas.height )
		.AddKeyValuePair( "tileWidth", m_Canvas.tileWidth )
		.AddKeyValuePair( "tileHeight", m_Canvas.tileHeight )
		.EndObject() // Canvas
		.AddKeyValuePair( "mapType",
						  ( m_eMapType == Scion::Core::EMapType::Grid ? std::string{ "grid" } : std::string{ "iso" } ) )
		.StartNewObject( "playerStart" )
		.AddKeyValuePair( "enabled", m_bUsePlayerStart )
		.AddKeyValuePair( "character", m_bUsePlayerStart ? m_PlayerStart.GetCharacterName() : std::string{ "default" } )
		.StartNewObject( "position" )
		.AddKeyValuePair( "x", playerStartPosition.x )
		.AddKeyValuePair( "y", playerStartPosition.y )
		.EndObject() // Player start position
		.EndObject() // Player Start
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

void Scene::SetCanvasOffset()
{
	if ( m_eMapType == Scion::Core::EMapType::Grid )
	{
		m_Canvas.offset = glm::vec2{ 0.f };
		return;
	}

	// TODO: ADD checks to ensure width is 2x height for iso tiles.

	// The width of the tile must be 2x the height
	// Right now we are hard coding it everywhere; however
	// this should automatically change when in iso.
	float doubleTileWidth = m_Canvas.tileWidth * 2.f;

	float halfTileWidth = doubleTileWidth / 2.f;
	float halfTileHeight = m_Canvas.tileHeight / 2.f;

	/* Find the X Offset */
	// Get the Hypotenuse of one tile
	float c1 = sqrt( ( halfTileWidth * halfTileWidth ) + ( halfTileHeight * halfTileHeight ) );

	// Get the length of the entire Y - Side hypotenuse
	float c2 = m_Canvas.width / doubleTileWidth * c1;

	// Find theta
	float theta1 = atan2( halfTileWidth, halfTileHeight );

	// Get the length of offset x
	float offsetX = sin( theta1 ) * c2;
	m_Canvas.offset.x = floor( offsetX );
	// We are assuming that there is no offset in Y currently
	m_Canvas.offset.y = 0.f;
}

void Scene::CreateLuaBind( sol::state& lua )
{
	lua.new_usertype<Canvas>( "Canvas",
							  sol::call_constructor,
							  sol::factories( [] { return Canvas{}; },
											  []( int width, int height, int tileWidth, int tileHeight ) {
												  return Canvas{ .width = width,
																 .height = height,
																 .tileWidth = tileWidth,
																 .tileHeight = tileHeight };
											  } ),
							  "width",
							  &Canvas::width,
							  "height",
							  &Canvas::height,
							  "tileWidth",
							  &Canvas::tileWidth,
							  "tileHeight",
							  &Canvas::tileHeight );
}

} // namespace Scion::Core
