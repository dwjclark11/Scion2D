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
	if (!tilemap.IsArray() || tilemap.Size() < 1)
	{
		SCION_ERROR( "Failed to load tilemap: File: [{}] - There needs to be at least 1 tile.", sTilemapFile );
		return false;
	}

	for (const auto& tile : tilemap.GetArray())
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
		DESERIALIZE_COMPONENT( jsonSprite, sprite);

		if (components.HasMember("boxCollider"))
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

} // namespace SCION_CORE::Loaders
