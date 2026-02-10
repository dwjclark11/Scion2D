#include "editor/loaders/TiledMapImporter.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MainRegistry.h"

#include "Rendering/Essentials/Texture.h"

#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"

#include <tinyxml2.h>

using namespace Scion::Core;
using namespace Scion::Core::ECS;
using namespace Scion::Physics;
using namespace tinyxml2;

namespace Scion::Editor
{

bool TiledMapImporter::ImportTilemapFromTiled( EditorSceneManager* pSceneManager, const std::string sTiledMapFile )
{
	fs::path filepath{ sTiledMapFile };
	if ( filepath.extension() == ".lua" )
		return ImportFromLuaFile( pSceneManager, sTiledMapFile );
	else if ( filepath.extension() == ".tmx" )
		return ImportFromTMXFile( pSceneManager, sTiledMapFile );

	SCION_ERROR( "Failed to import tiled map. File [{}] is not supported.", sTiledMapFile );
	return false;
}

bool TiledMapImporter::ImportFromTMXFile( EditorSceneManager* pSceneManager, const std::string sTiledMapFile )
{
	auto pXMLDoc = std::make_unique<tinyxml2::XMLDocument>();
	if ( !pXMLDoc )
	{
		return false;
	}

	if ( pXMLDoc->LoadFile( sTiledMapFile.c_str() ) != XML_SUCCESS )
	{
		std::string sError{ pXMLDoc->ErrorStr() };
		SCION_ERROR( "Failed to load tiled map file [{}] - {}", sTiledMapFile, sError );
		return false;
	}

	auto& coreGlobals = CORE_GLOBALS();
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	SceneObject newScene{ fs::path{ sTiledMapFile }.stem().string(), Scion::Core::EMapType::Grid };
	auto& canvas = newScene.GetCanvas();

	// Get the Root Element
	XMLElement* pRootElement = pXMLDoc->RootElement();
	if ( !pRootElement )
	{
		std::string sError{ pXMLDoc->ErrorStr() };
		SCION_ERROR( "Failed to load tiled map file [{}] - {}", sTiledMapFile, sError );
		return false;
	}

	XMLElement* pTileset = pRootElement->FirstChildElement( "tileset" );
	if ( !pTileset )
	{
		std::string sError{ pXMLDoc->ErrorStr() };
		SCION_ERROR( "Failed to load tiled map file [{}] - {}", sTiledMapFile, sError );
		return false;
	}

	// Get all tilesets
	std::vector<Tileset> tilesets;

	// Load all of the tileset info
	while ( pTileset )
	{
		std::vector<Tile> tiles;
		XMLElement* pTile = pTileset->FirstChildElement( "tile" );
		while ( pTile )
		{
			Tile newTile{};
			newTile.id = std::atoi( pTile->Attribute( "id" ) );

			XMLElement* pObjectGroup = pTile->FirstChildElement( "objectgroup" );
			XMLElement* pObject = pObjectGroup->FirstChildElement( "object" );

			while ( pObject )
			{
				TileObject tileObject{};
				tileObject.sName = pObject->Attribute( "name" ) ? pObject->Attribute( "name" ) : "";
				tileObject.sType = pObject->Attribute( "type" ) ? pObject->Attribute( "type" ) : "";
				tileObject.width = std::atoi( pObject->Attribute( "width" ) );
				tileObject.height = std::atoi( pObject->Attribute( "height" ) );
				tileObject.position =
					glm::vec2{ std::atof( pObject->Attribute( "x" ) ), std::atof( pObject->Attribute( "x" ) ) };

				newTile.tileObjects.emplace_back( std::move( tileObject ) );
				pObject = pObject->NextSiblingElement( "object" );
			}

			tiles.emplace_back( std::move( newTile ) );

			// Move to the next tile
			pTile = pTile->NextSiblingElement( "tile" );
		}

		Tileset newTileset{
			.sName = pTileset->Attribute( "name" ) ? pTileset->Attribute( "name" ) : "",
			.columns = pTileset->Attribute( "columns" ) ? std::atoi( pTileset->Attribute( "columns" ) ) : -1,
			.width = pTileset->Attribute( "width" ) ? std::atoi( pTileset->Attribute( "width" ) ) : 0,
			.height = pTileset->Attribute( "height" ) ? std::atoi( pTileset->Attribute( "height" ) ) : 0,
			.tileWidth = pTileset->Attribute( "tilewidth" ) ? std::atoi( pTileset->Attribute( "tilewidth" ) ) : 16,
			.tileHeight = pTileset->Attribute( "tileheight" ) ? std::atoi( pTileset->Attribute( "tileheight" ) ) : 16,
			.firstGID = pTileset->Attribute( "firstgid" ) ? std::atoi( pTileset->Attribute( "firstgid" ) ) : 1,
			.tileCount = pTileset->Attribute( "tilecount" ) ? std::atoi( pTileset->Attribute( "tilecount" ) ) : 1,
			.tiles = std::move( tiles )
		};

		if (newTileset.sName.empty() || newTileset.columns <= 0)
		{
			SCION_ERROR( "Failed to load tileset" );
			return false;
		}

		newTileset.SetRows();
		tilesets.emplace_back( std::move( newTileset ) );

		// Move to the next tileset
		pTileset = pTileset->NextSiblingElement( "tileset" );
	}

	XMLElement* pLayer = pRootElement->FirstChildElement( "layer" );
	int LAYER = 0;
	while (pLayer)
	{
		const int ROWS = std::atoi( pLayer->Attribute( "height" ) ) - 1;
		const int COLS = std::atoi( pLayer->Attribute( "width" ) );
		const std::string sLayerName = pLayer->Attribute( "name" );

		newScene.AddLayer( Scion::Utilities::SpriteLayerParams{
			.sLayerName = sLayerName,
			.bVisible = true,
			.layer = LAYER,
		} );

		XMLElement* pLayerData = pLayer->FirstChildElement( "data" );
		const char* dataCSV = pLayerData->GetText();

		std::vector<int> tileIDs;
		std::stringstream ss( dataCSV );
		std::string value;

		// Parse the data
		while (std::getline(ss, value, ','))
		{
			if (!value.empty())
			{
				tileIDs.push_back( std::stoi( value ) );
			}
		}

		for (int row = 0; row < ROWS; row++)
		{
			for (int col = 1; col < COLS; col++)
			{
				int id = tileIDs[ row * COLS + col ];
				// If the ID is zero, we can skip, there is no tile there.
				if (id == 0)
					continue;

				if ( auto* pTileset = GetTileset( tilesets, id ) )
				{
					Entity newTile{ newScene.GetRegistryPtr(), "", "" };
					auto& transform = newTile.AddComponent<TransformComponent>();
					transform.position = glm::vec2{ ( col - 1 ) * pTileset->tileWidth, row * pTileset->tileHeight };

					// Add the box collider if there is an object
					if ( auto* pObject = pTileset->GetObjectFromId( id ) )
					{
						auto& boxCollider = newTile.AddComponent<BoxColliderComponent>();
						boxCollider.width = pObject->width;
						boxCollider.height = pObject->height;
						boxCollider.offset = pObject->position;

						if ( coreGlobals.IsPhysicsEnabled() )
						{
							PhysicsAttributes physAttr{};
							physAttr.eType = Scion::Physics::RigidBodyType::STATIC;
							physAttr.density = 1000.f;
							physAttr.friction = 0.f;
							physAttr.restitution = 0.f;
							physAttr.position = transform.position + pObject->position;
							physAttr.bFixedRotation = true;
							physAttr.boxSize = glm::vec2{ boxCollider.width, boxCollider.height };
							physAttr.bBoxShape = true;

							Scion::Physics::ObjectData objectData{};
							objectData.tag = pObject->sName;
							objectData.group = pObject->sType;
							objectData.entityID = static_cast<uint32_t>( newTile.GetEntity() );

							newTile.AddComponent<PhysicsComponent>( PhysicsComponent{ physAttr } );
						}
					}
					auto pTexture = assetManager.GetTexture( pTileset->sName );
					SCION_ASSERT(
						pTexture &&
						"All tiles must have a texture. Texture Must be loaded into asset manager before import" );
					if ( !pTexture )
					{
						SCION_ERROR( "Failed to import tiled map. Texture [{}] must be loaded in the asset manager "
									 "prior to import.",
									 pTileset->sName );
						return false;
					}

					auto [ startX, startY ] = pTileset->GetTileStartXY( id );

					auto& sprite = newTile.AddComponent<SpriteComponent>();
					sprite.sTextureName = pTileset->sName;
					sprite.width = pTileset->tileWidth;
					sprite.height = pTileset->tileHeight;
					sprite.start_x = startX;
					sprite.start_y = startY;
					sprite.layer = LAYER;

					Scion::Core::GenerateUVs( sprite, pTexture->GetWidth(), pTexture->GetHeight() );

					newTile.AddComponent<TileComponent>(
						TileComponent{ .id = static_cast<uint32_t>( newTile.GetEntity() ) } );
				}
			}
		}

		// Move to the next layer
		pLayer = pLayer->NextSiblingElement( "layer" );
		LAYER++;
	}

	newScene.SaveScene( true );
	return pSceneManager->AddSceneObject( newScene.GetName(), newScene.GetSceneDataPath() );
}

bool TiledMapImporter::ImportFromLuaFile( EditorSceneManager* pSceneManager, const std::string sTiledMapFile )
{
	auto& coreGlobals = CORE_GLOBALS();
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	sol::state lua;
	lua.open_libraries( sol::lib::base );
	sol::table map;
	try
	{
		map = lua.safe_script_file( sTiledMapFile );
	}
	catch ( const sol::error& err )
	{
		SCION_ERROR( "Failed to import Tiled map. {}", err.what() );
		return false;
	}

	if ( !map.valid() )
	{
		SCION_ERROR( "Failed to import Tiled map. Map table invalid. {}" );
		return false;
	}

	SceneObject newScene{ fs::path{ sTiledMapFile }.stem().string(), Scion::Core::EMapType::Grid };
	auto& canvas = newScene.GetCanvas();

	canvas.tileWidth = map[ "tilewidth" ].get_or( 16 );
	canvas.tileHeight = map[ "tileheight" ].get_or( 16 );
	canvas.width = map[ "width" ].get_or( 20 ) * canvas.tileWidth;

	// Get all tilesets
	std::vector<Tileset> tilesets;
	sol::optional<sol::table> optTilesets = map[ "tilesets" ];
	if ( optTilesets )
	{
		for ( const auto& [ _, tileset ] : *optTilesets )
		{
			sol::table tilesetTbl = tileset.as<sol::table>();
			sol::optional<sol::table> optTilesetTiles = tilesetTbl[ "tiles" ];
			std::vector<Tile> tiles;

			if ( optTilesetTiles )
			{
				for ( const auto& [ index, tileObj ] : *optTilesetTiles )
				{
					sol::table tileTbl = tileObj.as<sol::table>();
					Tile newTile{};
					newTile.id = tileTbl[ "id" ].get<int>();
					sol::table objects = tileTbl[ "objectGroup" ][ "objects" ].get<sol::table>();
					for ( const auto& [ objIndex, objectObj ] : objects )
					{
						sol::table objectTbl = objectObj.as<sol::table>();
						TileObject tileObject{
							.sName = objectTbl[ "name" ].get_or( std::string{} ),
							.sType = objectTbl[ "type" ].get_or( std::string{} ),
							.position = glm::vec2{ objectTbl[ "x" ].get_or( 0.f ), objectTbl[ "y" ].get_or( 0.f ) },
							.width = objectTbl[ "width" ].get_or( 16.f ),
							.height = objectTbl[ "height" ].get_or( 16.f ),
							.rotation = objectTbl[ "rotation" ].get_or( 0.f ) };

						newTile.tileObjects.emplace_back( std::move( tileObject ) );
					}

					tiles.emplace_back( std::move( newTile ) );
				}
			}

			Tileset newTileset{ .sName = tilesetTbl[ "name" ].get_or( std::string{} ),
								.columns = tilesetTbl[ "columns" ].get_or( -1 ),
								.width = tilesetTbl[ "imagewidth" ].get_or( 0 ),
								.height = tilesetTbl[ "imageheight" ].get_or( 0 ),
								.tileWidth = tilesetTbl[ "tilewidth" ].get_or( 16 ),
								.tileHeight = tilesetTbl[ "tileheight" ].get_or( 16 ),
								.firstGID = tilesetTbl[ "firstgid" ].get_or( 1 ),
								.tileCount = tilesetTbl[ "tilecount" ].get_or( 1 ),
								.tiles = std::move( tiles ) };

			if ( newTileset.sName.empty() || newTileset.columns <= 0 )
			{
				SCION_ERROR( "Failed to load tileset" );
				return false;
			}

			newTileset.SetRows();
			tilesets.emplace_back( newTileset );
		}
	}

	sol::optional<sol::table> optMapLayers = map[ "layers" ];
	if ( optMapLayers )
	{
		for ( const auto& [ index, layerObj ] : *optMapLayers )
		{
			sol::table layer = layerObj.as<sol::table>();
			const int ROWS = layer[ "height" ].get<int>() - 1;
			const int COLS = layer[ "width" ].get<int>();
			const int LAYER = index.as<int>();

			const std::string sLayerName = layer[ "name" ].get<std::string>();
			bool bVisible = layer[ "visible" ].get<bool>();

			newScene.AddLayer( Scion::Utilities::SpriteLayerParams{
				.sLayerName = sLayerName,
				.bVisible = bVisible,
				.layer = LAYER,
			} );

			sol::table layerData = layer[ "data" ];

			for ( int row = 0; row < ROWS; row++ )
			{
				for ( int col = 1; col < COLS; col++ )
				{
					int id = layerData[ row * COLS + col ].get<int>();
					if ( id == 0 )
						continue;

					if ( auto* pTileset = GetTileset(tilesets, id ) )
					{
						Entity newTile{ newScene.GetRegistryPtr(), "", "" };
						auto& transform = newTile.AddComponent<TransformComponent>();
						transform.position = glm::vec2{ ( col - 1 ) * pTileset->tileWidth, row * pTileset->tileHeight };

						// Add the box collider if there is an object
						if ( auto* pObject = pTileset->GetObjectFromId( id ) )
						{
							auto& boxCollider = newTile.AddComponent<BoxColliderComponent>();
							boxCollider.width = pObject->width;
							boxCollider.height = pObject->height;
							boxCollider.offset = pObject->position;

							if ( coreGlobals.IsPhysicsEnabled() )
							{
								PhysicsAttributes physAttr{};
								physAttr.eType = Scion::Physics::RigidBodyType::STATIC;
								physAttr.density = 1000.f;
								physAttr.friction = 0.f;
								physAttr.restitution = 0.f;
								physAttr.position = transform.position + pObject->position;
								physAttr.bFixedRotation = true;
								physAttr.boxSize = glm::vec2{ boxCollider.width, boxCollider.height };
								physAttr.bBoxShape = true;

								Scion::Physics::ObjectData objectData{};
								objectData.tag = pObject->sName;
								objectData.group = pObject->sType;
								objectData.entityID = static_cast<uint32_t>( newTile.GetEntity() );

								newTile.AddComponent<PhysicsComponent>( PhysicsComponent{ physAttr } );
							}
						}
						auto pTexture = assetManager.GetTexture( pTileset->sName );
						SCION_ASSERT(
							pTexture &&
							"All tiles must have a texture. Texture Must be loaded into asset manager before import" );
						if ( !pTexture )
						{
							SCION_ERROR( "Failed to import tiled map. Texture [{}] must be loaded in the asset manager "
										 "prior to import.",
										 pTileset->sName );
							return false;
						}

						auto [ startX, startY ] = pTileset->GetTileStartXY( id );

						auto& sprite = newTile.AddComponent<SpriteComponent>();
						sprite.sTextureName = pTileset->sName;
						sprite.width = pTileset->tileWidth;
						sprite.height = pTileset->tileHeight;
						sprite.start_x = startX;
						sprite.start_y = startY;
						sprite.layer = LAYER;

						Scion::Core::GenerateUVs( sprite, pTexture->GetWidth(), pTexture->GetHeight() );

						newTile.AddComponent<TileComponent>(
							TileComponent{ .id = static_cast<uint32_t>( newTile.GetEntity() ) } );
					}
				}
			}
		}
	}

	newScene.SaveScene( true );
	return pSceneManager->AddSceneObject( newScene.GetName(), newScene.GetSceneDataPath() );
}

TiledMapImporter::Tileset* TiledMapImporter::GetTileset( std::vector<Tileset>& tilesets, int id )
{
	for ( int i = 0; i < tilesets.size(); i++ )
	{
		auto& tileset = tilesets[ i ];
		if ( tileset.TileIdExists( id ) )
		{
			return &tileset;
		}
	}

	return nullptr;
}

std::tuple<int, int> TiledMapImporter::Tileset::GetTileStartXY( int id )
{
	if ( !TileIdExists( id ) )
		return std::make_tuple( -1, -1 );

	int tileID = id - firstGID;
	int startY = tileID / columns;
	int startX = tileID % columns;

	return std::make_tuple( startX, startY );
}

TiledMapImporter::TileObject* TiledMapImporter::Tileset::GetObjectFromId( int id )
{
	int actualID = id - firstGID;
	for ( auto& tile : tiles )
	{
		if ( tile.id == actualID )
		{
			if ( !tile.tileObjects.empty() )
			{
				// Return the first object.
				return &tile.tileObjects[ 0 ];
			}
		}
	}

	return nullptr;
}

} // namespace Scion::Editor
