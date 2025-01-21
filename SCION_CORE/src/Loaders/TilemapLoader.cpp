#include "Core/Loaders/TilemapLoader.h"
#include "Core/ECS/Components/ComponentSerializer.h"
#include "Core/ECS/Registry.h"
#include "Core/ECS/Entity.h"
#include "ScionFilesystem/Serializers/JSONSerializer.h"
#include "Logger/Logger.h"
#include <rapidjson/error/en.h>
#include <filesystem>

using namespace SCION_FILESYSTEM;
using namespace SCION_CORE::ECS;

namespace fs = std::filesystem;

namespace SCION_CORE::Loaders
{
bool TilemapLoader::SaveTilemapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile )
{
	std::unique_ptr<JSONSerializer> pSerializer{ nullptr };

	try
	{
		pSerializer = std::make_unique<JSONSerializer>( sTilemapFile );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to save tilemap [{}] - [{}]", sTilemapFile, ex.what() );
		return false;
	}

	fs::path tilemapPath{ sTilemapFile };
	if ( !fs::exists( tilemapPath ) )
	{
		SCION_ERROR( "Failed to save tilemap - Filepath does not exist [{}]", sTilemapFile );
		return false;
	}

	pSerializer->StartDocument();
	pSerializer->StartNewArray( "tilemap" );

	auto tiles = registry.GetRegistry().view<TileComponent>();

	for ( auto tile : tiles )
	{
		pSerializer->StartNewObject();
		pSerializer->StartNewObject( "components" );
		auto tileEnt{ Entity{ registry, tile } };

		const auto& transform = tileEnt.GetComponent<TransformComponent>();
		SERIALIZE_COMPONENT( *pSerializer, transform );

		const auto& sprite = tileEnt.GetComponent<SpriteComponent>();
		SERIALIZE_COMPONENT( *pSerializer, sprite );

		if ( tileEnt.HasComponent<BoxColliderComponent>() )
		{
			const auto& boxCollider = tileEnt.GetComponent<BoxColliderComponent>();
			SERIALIZE_COMPONENT( *pSerializer, boxCollider );
		}

		if ( tileEnt.HasComponent<CircleColliderComponent>() )
		{
			const auto& circleCollider = tileEnt.GetComponent<CircleColliderComponent>();
			SERIALIZE_COMPONENT( *pSerializer, circleCollider );
		}

		if ( tileEnt.HasComponent<AnimationComponent>() )
		{
			const auto& animation = tileEnt.GetComponent<AnimationComponent>();
			SERIALIZE_COMPONENT( *pSerializer, animation );
		}

		if ( tileEnt.HasComponent<PhysicsComponent>() )
		{
			const auto& physics = tileEnt.GetComponent<PhysicsComponent>();
			SERIALIZE_COMPONENT( *pSerializer, physics );
		}

		pSerializer->EndObject(); // Components object
		pSerializer->EndObject(); // tile object
	}

	pSerializer->EndArray(); // Tilemap array
	return pSerializer->EndDocument();
}

bool TilemapLoader::LoadTilemapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile )
{
	std::ifstream mapFile;
	mapFile.open( sTilemapFile );

	if ( !mapFile.is_open() )
	{
		SCION_ERROR( "Failed to open tilemap file [{}]", sTilemapFile );
		return false;
	}

	// The tilemap file could be empty if just created
	if ( mapFile.peek() == std::ifstream::traits_type::eof() )
	{
		// If the tilemap is an empty file, return true. must not have made any
		// tiles yet.
		return true;
	}

	std::stringstream ss;
	ss << mapFile.rdbuf();
	std::string contents = ss.str();
	rapidjson::StringStream jsonStr{ contents.c_str() };

	rapidjson::Document doc;
	doc.ParseStream( jsonStr );

	if ( doc.HasParseError() || !doc.IsObject() )
	{
		SCION_ERROR( "Failed to load tilemap: File: [{}] is not valid JSON. - {} - {}",
					 sTilemapFile,
					 rapidjson::GetParseError_En( doc.GetParseError() ),
					 doc.GetErrorOffset() );
		return false;
	}

	const rapidjson::Value& tilemap = doc[ "tilemap" ];
	if ( !tilemap.IsArray() || tilemap.Size() < 1 )
	{
		SCION_ERROR( "Failed to load tilemap: File: [{}] - There needs to be at least 1 tile.", sTilemapFile );
		return false;
	}

	for ( const auto& tile : tilemap.GetArray() )
	{
		Entity newTile{ registry, "", "" };
		const auto& components = tile[ "components" ];

		// Transform
		const auto& jsonTransform = components[ "transform" ];
		auto& transform = newTile.AddComponent<TransformComponent>();
		DESERIALIZE_COMPONENT( jsonTransform, transform );

		// Sprite
		const auto& jsonSprite = components[ "sprite" ];
		auto& sprite = newTile.AddComponent<SpriteComponent>();
		DESERIALIZE_COMPONENT( jsonSprite, sprite );

		if ( components.HasMember( "boxCollider" ) )
		{
			const auto& jsonBoxCollider = components[ "boxCollider" ];
			auto& boxCollider = newTile.AddComponent<BoxColliderComponent>();
			DESERIALIZE_COMPONENT( jsonBoxCollider, boxCollider );
		}

		if ( components.HasMember( "circleCollider" ) )
		{
			const auto& jsonCircleCollider = components[ "circleCollider" ];
			auto& circleCollider = newTile.AddComponent<CircleColliderComponent>();
			DESERIALIZE_COMPONENT( jsonCircleCollider, circleCollider );
		}

		if ( components.HasMember( "animation" ) )
		{
			const auto& jsonAnimation = components[ "animation" ];
			auto& animation = newTile.AddComponent<AnimationComponent>();
			DESERIALIZE_COMPONENT( jsonAnimation, animation );
		}

		if ( components.HasMember( "physics" ) )
		{
			const auto& jsonPhysics = components[ "physics" ];
			auto& physics = newTile.AddComponent<PhysicsComponent>();
			DESERIALIZE_COMPONENT( jsonPhysics, physics );
		}

		newTile.AddComponent<TileComponent>( TileComponent{ .id = static_cast<uint32_t>( newTile.GetEntity() ) } );
	}

	mapFile.close();
	return true;
}

bool TilemapLoader::SaveObjectMapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile )
{
	std::unique_ptr<JSONSerializer> pSerializer{ nullptr };

	try
	{
		pSerializer = std::make_unique<JSONSerializer>( sObjectMapFile );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to save game objects[{}] - [{}]", sObjectMapFile, ex.what() );
		return false;
	}

	fs::path objectPath{ sObjectMapFile };
	if ( !fs::exists( objectPath ) )
	{
		SCION_ERROR( "Failed to save game objects - Filepath does not exist [{}]", sObjectMapFile );
		return false;
	}

	pSerializer->StartDocument();
	pSerializer->StartNewArray( "game_objects" );

	auto gameObjects = registry.GetRegistry().view<entt::entity>(entt::exclude<TileComponent>);

	for ( auto object : gameObjects )
	{
		pSerializer->StartNewObject();
		pSerializer->StartNewObject( "components" );
		auto objectEnt{ Entity{ registry, object } };

		if ( const auto* id = objectEnt.TryGetComponent<Identification>() )
		{
			SERIALIZE_COMPONENT( *pSerializer, *id );
		}


		if ( const auto* transform = objectEnt.TryGetComponent<TransformComponent>() )
		{
			SERIALIZE_COMPONENT( *pSerializer, *transform );
		}

		if ( const auto* sprite = objectEnt.TryGetComponent<SpriteComponent>() )
		{
			SERIALIZE_COMPONENT( *pSerializer, *sprite );
		}

		
		if ( objectEnt.HasComponent<BoxColliderComponent>() )
		{
			const auto& boxCollider = objectEnt.GetComponent<BoxColliderComponent>();
			SERIALIZE_COMPONENT( *pSerializer, boxCollider );
		}

		if ( objectEnt.HasComponent<CircleColliderComponent>() )
		{
			const auto& circleCollider = objectEnt.GetComponent<CircleColliderComponent>();
			SERIALIZE_COMPONENT( *pSerializer, circleCollider );
		}

		if ( objectEnt.HasComponent<AnimationComponent>() )
		{
			const auto& animation = objectEnt.GetComponent<AnimationComponent>();
			SERIALIZE_COMPONENT( *pSerializer, animation );
		}

		if ( objectEnt.HasComponent<PhysicsComponent>() )
		{
			const auto& physics = objectEnt.GetComponent<PhysicsComponent>();
			SERIALIZE_COMPONENT( *pSerializer, physics );
		}

		pSerializer->EndObject(); // Components object
		pSerializer->EndObject(); // Ent GameObject object
	}

	pSerializer->EndArray(); // GameObjects array
	return pSerializer->EndDocument();
}

bool TilemapLoader::LoadObjectMapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile )
{
	std::ifstream mapFile;
	mapFile.open( sObjectMapFile );

	if ( !mapFile.is_open() )
	{
		SCION_ERROR( "Failed to open tilemap file [{}]", sObjectMapFile );
		return false;
	}

	// The object file could be empty if just created
	if ( mapFile.peek() == std::ifstream::traits_type::eof() )
	{
		// If the object is an empty file, return true. must not have made any
		// objects yet.
		return true;
	}

	std::stringstream ss;
	ss << mapFile.rdbuf();
	std::string contents = ss.str();
	rapidjson::StringStream jsonStr{ contents.c_str() };

	rapidjson::Document doc;
	doc.ParseStream( jsonStr );

	if ( doc.HasParseError() || !doc.IsObject() )
	{
		SCION_ERROR( "Failed to load tilemap: File: [{}] is not valid JSON. - {} - {}",
					 sObjectMapFile,
					 rapidjson::GetParseError_En( doc.GetParseError() ),
					 doc.GetErrorOffset() );
		return false;
	}

	const rapidjson::Value& gameObjects = doc[ "game_objects" ];
	if ( !gameObjects.IsArray() || gameObjects.Size() < 1 )
	{
		SCION_ERROR( "Failed to load Game Objects: File: [{}] - There needs to be at least 1 tile.", sObjectMapFile );
		return false;
	}

	for ( const auto& object : gameObjects.GetArray() )
	{
		Entity gameObject{ registry, "", "" };
		const auto& components = object[ "components" ];

		// Transform
		const auto& jsonTransform = components[ "transform" ];
		auto& transform = gameObject.AddComponent<TransformComponent>();
		DESERIALIZE_COMPONENT( jsonTransform, transform );

		// Sprite -- We cannot assume that all game objects have a sprite
		if ( components.HasMember( "sprite" ) )
		{
			auto& jsonSprite = components[ "sprite" ];
			auto& sprite = gameObject.AddComponent<SpriteComponent>();
			DESERIALIZE_COMPONENT( jsonSprite, sprite );
		}

		if ( components.HasMember( "boxCollider" ) )
		{
			const auto& jsonBoxCollider = components[ "boxCollider" ];
			auto& boxCollider = gameObject.AddComponent<BoxColliderComponent>();
			DESERIALIZE_COMPONENT( jsonBoxCollider, boxCollider );
		}

		if ( components.HasMember( "circleCollider" ) )
		{
			const auto& jsonCircleCollider = components[ "circleCollider" ];
			auto& circleCollider = gameObject.AddComponent<CircleColliderComponent>();
			DESERIALIZE_COMPONENT( jsonCircleCollider, circleCollider );
		}

		if ( components.HasMember( "animation" ) )
		{
			const auto& jsonAnimation = components[ "animation" ];
			auto& animation = gameObject.AddComponent<AnimationComponent>();
			DESERIALIZE_COMPONENT( jsonAnimation, animation );
		}

		if ( components.HasMember( "physics" ) )
		{
			const auto& jsonPhysics = components[ "physics" ];
			auto& physics = gameObject.AddComponent<PhysicsComponent>();
			DESERIALIZE_COMPONENT( jsonPhysics, physics );
		}

		if (components.HasMember("id"))
		{
			const auto& jsonID = components[ "id" ];
			auto& id = gameObject.GetComponent<Identification>();
			DESERIALIZE_COMPONENT( jsonID, id );
		}

		// TODO: Handle Relationships???
	}

	mapFile.close();
	return true;
}

bool TilemapLoader::SaveTilemap( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON )
{
	if ( bUseJSON )
		return SaveTilemapJSON( registry, sTilemapFile );

	return false;
}
bool TilemapLoader::LoadTilemap( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON )
{
	if ( bUseJSON )
		return LoadTilemapJSON( registry, sTilemapFile );

	return false;
}

bool TilemapLoader::LoadGameObjects( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile,
									 bool bUseJSON )
{
	if ( bUseJSON )
		return LoadObjectMapJSON( registry, sObjectMapFile );

	return false;
}

bool TilemapLoader::SaveGameObjects( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile,
									 bool bUseJSON )
{
	if ( bUseJSON )
		return SaveObjectMapJSON( registry, sObjectMapFile );

	return false;
}

} // namespace SCION_CORE::Loaders
