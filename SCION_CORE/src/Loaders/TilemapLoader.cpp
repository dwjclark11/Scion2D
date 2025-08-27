#include "Core/Loaders/TilemapLoader.h"
#include "Core/ECS/Components/ComponentSerializer.h"
#include "Core/ECS/Registry.h"
#include "Core/ECS/Entity.h"
#include "ScionFilesystem/Serializers/JSONSerializer.h"
#include "ScionFilesystem/Serializers/LuaSerializer.h"
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
		SCION_WARN( "Failed to load tilemap: File: [{}] - There needs to be at least 1 tile.", sTilemapFile );
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

	auto gameObjects = registry.GetRegistry().view<entt::entity>( entt::exclude<TileComponent, UneditableComponent> );

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

		if ( objectEnt.HasComponent<TextComponent>() )
		{
			const auto& text = objectEnt.GetComponent<TextComponent>();
			SERIALIZE_COMPONENT( *pSerializer, text );
		}

		if ( objectEnt.HasComponent<UIComponent>() )
		{
			const auto& ui = objectEnt.GetComponent<UIComponent>();
			SERIALIZE_COMPONENT( *pSerializer, ui );
		}

		if ( auto* relations = objectEnt.TryGetComponent<Relationship>() )
		{
			pSerializer->StartNewObject( "relationship" );
			if ( relations->parent != entt::null )
			{
				Entity parent{ registry, relations->parent };
				pSerializer->AddKeyValuePair( "parent", parent.GetName() );
			}
			else
			{
				pSerializer->AddKeyValuePair( "parent", std::string{} );
			}

			if ( relations->nextSibling != entt::null )
			{
				Entity nextSibling{ registry, relations->nextSibling };
				pSerializer->AddKeyValuePair( "nextSibling", nextSibling.GetName() );
			}
			else
			{
				pSerializer->AddKeyValuePair( "nextSibling", std::string{} );
			}

			if ( relations->prevSibling != entt::null )
			{
				Entity prevSibling{ registry, relations->prevSibling };
				pSerializer->AddKeyValuePair( "prevSibling", prevSibling.GetName() );
			}
			else
			{
				pSerializer->AddKeyValuePair( "prevSibling", std::string{} );
			}

			if ( relations->firstChild != entt::null )
			{
				Entity firstChild{ registry, relations->firstChild };
				pSerializer->AddKeyValuePair( "firstChild", firstChild.GetName() );
			}
			else
			{
				pSerializer->AddKeyValuePair( "firstChild", std::string{} );
			}
			pSerializer->EndObject(); // Relationship Object
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
		SCION_WARN( "Failed to load Game Objects: File: [{}] - There needs to be at least 1 tile.", sObjectMapFile );
		return false;
	}

	// Map of entity to relationships
	std::map<entt::entity, SaveRelationship> mapEntityToRelationship;

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

		if ( components.HasMember( "id" ) )
		{
			const auto& jsonID = components[ "id" ];
			auto& id = gameObject.GetComponent<Identification>();
			DESERIALIZE_COMPONENT( jsonID, id );
		}

		if ( components.HasMember( "text" ) )
		{
			const auto& jsonText = components[ "text" ];
			auto& text = gameObject.AddComponent<TextComponent>();
			DESERIALIZE_COMPONENT( jsonText, text );
		}

		if ( components.HasMember( "ui" ) )
		{
			const auto& jsonUI = components[ "ui" ];
			auto& ui = gameObject.AddComponent<UIComponent>();
			DESERIALIZE_COMPONENT( jsonUI, ui );
		}

		if ( components.HasMember( "relationship" ) )
		{
			const rapidjson::Value& relations = components[ "relationship" ];
			SaveRelationship saveRelations{};
			saveRelations.sParent = relations[ "parent" ].GetString();
			saveRelations.sNextSibling = relations[ "nextSibling" ].GetString();
			saveRelations.sPrevSibling = relations[ "prevSibling" ].GetString();
			saveRelations.sFirstChild = relations[ "firstChild" ].GetString();

			mapEntityToRelationship.emplace( gameObject.GetEntity(), saveRelations );
		}
	}

	auto ids = registry.GetRegistry().view<Identification>( entt::exclude<TileComponent> );

	auto findTag = [ & ]( const std::string& sTag ) {
		auto parItr = std::ranges::find_if( ids, [ & ]( const auto& e ) {
			Entity en{ registry, e };
			return en.GetName() == sTag;
		} );

		if ( parItr != ids.end() )
		{
			return *parItr;
		}

		return entt::entity{ entt::null };
	};

	for ( auto& [ entity, saveRelations ] : mapEntityToRelationship )
	{
		Entity ent{ registry, entity };
		auto& relations = ent.GetComponent<Relationship>();

		// Find the parent
		if ( !saveRelations.sParent.empty() )
		{
			relations.parent = findTag( saveRelations.sParent );
		}

		// Find the nextSibling
		if ( !saveRelations.sNextSibling.empty() )
		{
			relations.nextSibling = findTag( saveRelations.sNextSibling );
		}

		// Find the prevSibling
		if ( !saveRelations.sPrevSibling.empty() )
		{
			relations.prevSibling = findTag( saveRelations.sPrevSibling );
		}

		// Find the firstChild
		if ( !saveRelations.sFirstChild.empty() )
		{
			relations.firstChild = findTag( saveRelations.sFirstChild );
		}
	}

	mapFile.close();
	return true;
}

bool TilemapLoader::SaveTilemapLua( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile )
{
	std::unique_ptr<LuaSerializer> pSerializer{ nullptr };

	try
	{
		pSerializer = std::make_unique<LuaSerializer>( sTilemapFile );
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

	pSerializer->StartNewTable( tilemapPath.stem().string() );
	pSerializer->StartNewTable( "tilemap" );

	auto tiles = registry.GetRegistry().view<TileComponent>();

	for ( auto tile : tiles )
	{
		pSerializer->StartNewTable();
		pSerializer->StartNewTable( "components" );
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

		pSerializer->EndTable(); // Components object
		pSerializer->EndTable(); // tile object
	}

	pSerializer->EndTable(); // Tilemap array
	pSerializer->EndTable(); // Scene Name
	return pSerializer->FinishStream();
}

bool TilemapLoader::LoadTilemapLua( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile )
{
	sol::state lua;
	try
	{
		lua.safe_script_file( sTilemapFile );
	}
	catch ( const sol::error& err )
	{
		SCION_ERROR( "Failed to load tilemap map file: [{}], {}", sTilemapFile, err.what() );
		return false;
	}

	sol::optional<sol::table> maybeTiles = lua[ "tilemap" ];
	if ( !maybeTiles )
	{
		SCION_ERROR( "Failed to load tilemap map file: \"tilemap\" table is missing or invalid." );
		return false;
	}

	for ( const auto& [ key, value ] : *maybeTiles )
	{
		Entity newTile{ registry, "", "" };
		const sol::optional<sol::table> components = value.as<sol::table>()[ "components" ];

		if ( !components )
		{
			SCION_ERROR( "Failed to load object map file: \"components\" table is missing or invalid." );
			return false;
		}

		// Transform
		const sol::table luaTransform = ( *components )[ "transform" ];
		auto& transform = newTile.AddComponent<TransformComponent>();
		DESERIALIZE_COMPONENT( luaTransform, transform );

		// Sprite
		const sol::table luaSprite = ( *components )[ "sprite" ];
		auto& sprite = newTile.AddComponent<SpriteComponent>();
		DESERIALIZE_COMPONENT( luaSprite, sprite );

		sol::optional<sol::table> luaBoxCollider = ( *components )[ "boxCollider" ];
		if ( luaBoxCollider )
		{
			auto& boxCollider = newTile.AddComponent<BoxColliderComponent>();
			DESERIALIZE_COMPONENT( *luaBoxCollider, boxCollider );
		}

		sol::optional<sol::table> luaCircleCollider = ( *components )[ "circleCollider" ];
		if ( luaCircleCollider )
		{
			auto& circleCollider = newTile.AddComponent<CircleColliderComponent>();
			DESERIALIZE_COMPONENT( *luaCircleCollider, circleCollider );
		}

		sol::optional<sol::table> luaAnimations = ( *components )[ "animation" ];
		if ( luaAnimations )
		{
			auto& animation = newTile.AddComponent<AnimationComponent>();
			DESERIALIZE_COMPONENT( *luaAnimations, animation );
		}

		sol::optional<sol::table> luaPhysics = ( *components )[ "physics" ];
		if ( luaPhysics )
		{
			auto& physics = newTile.AddComponent<PhysicsComponent>();
			DESERIALIZE_COMPONENT( *luaPhysics, physics );
		}

		newTile.AddComponent<TileComponent>( TileComponent{ .id = static_cast<uint32_t>( newTile.GetEntity() ) } );
	}

	return true;
}

bool TilemapLoader::SaveObjectMapLua( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile )
{
	std::unique_ptr<LuaSerializer> pSerializer{ nullptr };

	try
	{
		pSerializer = std::make_unique<LuaSerializer>( sObjectMapFile );
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

	pSerializer->StartNewTable( objectPath.stem().string() );
	pSerializer->StartNewTable( "game_objects" );

	auto gameObjects = registry.GetRegistry().view<entt::entity>( entt::exclude<TileComponent, UneditableComponent> );

	for ( auto object : gameObjects )
	{
		pSerializer->StartNewTable();
		pSerializer->StartNewTable( "components" );
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

		if ( auto* ui = objectEnt.TryGetComponent<UIComponent>() )
		{
			SERIALIZE_COMPONENT( *pSerializer, *ui );
		}

		if ( auto* text = objectEnt.TryGetComponent<TextComponent>() )
		{
			SERIALIZE_COMPONENT( *pSerializer, *text );
		}

		if ( auto* relations = objectEnt.TryGetComponent<Relationship>() )
		{
			pSerializer->StartNewTable( "relationship" );
			if ( relations->parent != entt::null )
			{
				Entity parent{ registry, relations->parent };
				pSerializer->AddKeyValuePair( "parent", parent.GetName(), false, false, false, true );
			}
			else
			{
				pSerializer->AddKeyValuePair( "parent", std::string{ "" }, false, false, false, true );
			}

			if ( relations->nextSibling != entt::null )
			{
				Entity nextSibling{ registry, relations->nextSibling };
				pSerializer->AddKeyValuePair( "nextSibling", nextSibling.GetName(), false, false, false, true );
			}
			else
			{
				pSerializer->AddKeyValuePair( "nextSibling", std::string{ "" }, false, false, false, true );
			}

			if ( relations->prevSibling != entt::null )
			{
				Entity prevSibling{ registry, relations->prevSibling };
				pSerializer->AddKeyValuePair( "prevSibling", prevSibling.GetName(), false, false, false, true );
			}
			else
			{
				pSerializer->AddKeyValuePair( "prevSibling", std::string{ "" }, false, false, false, true );
			}

			if ( relations->firstChild != entt::null )
			{
				Entity firstChild{ registry, relations->firstChild };
				pSerializer->AddKeyValuePair( "firstChild", firstChild.GetName(), false, false, false, true );
			}
			else
			{
				pSerializer->AddKeyValuePair( "firstChild", std::string{ "" }, false, true, false, true );
			}
			pSerializer->EndTable(); // Relationship Object
		}

		pSerializer->EndTable(); // Components object
		pSerializer->EndTable(); // End GameObject object
	}

	pSerializer->EndTable(); // GameObjects array
	pSerializer->EndTable(); // Main table

	return pSerializer->FinishStream();
}

bool TilemapLoader::LoadObjectMapLua( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile )
{
	sol::state lua;
	try
	{
		lua.safe_script_file( sObjectMapFile );
	}
	catch ( const sol::error& err )
	{
		SCION_ERROR( "Failed to load object map file: [{}], {}", sObjectMapFile, err.what() );
		return false;
	}

	// Map of entity to relationships
	std::map<entt::entity, SaveRelationship> mapEntityToRelationship;

	sol::optional<sol::table> maybeObjects = lua[ "game_objects" ];
	if ( !maybeObjects )
	{
		SCION_ERROR( "Failed to load object map file: \"game_objects\" table is missing or invalid." );
		return false;
	}

	for ( const auto& [ key, value ] : *maybeObjects )
	{
		Entity gameObject{ registry, "", "" };
		const sol::optional<sol::table> components = value.as<sol::table>()[ "components" ];

		if ( !components )
		{
			SCION_ERROR( "Failed to load object map file: \"components\" table is missing or invalid." );
			return false;
		}

		// Transform
		const sol::table luaTransform = ( *components )[ "transform" ];
		auto& transform = gameObject.AddComponent<TransformComponent>();
		DESERIALIZE_COMPONENT( luaTransform, transform );

		// Sprite -- We cannot assume that all game objects have a sprite
		const sol::optional<sol::table> luaSprite = ( *components )[ "transform" ];
		if ( luaSprite )
		{
			auto& sprite = gameObject.AddComponent<SpriteComponent>();
			DESERIALIZE_COMPONENT( *luaSprite, sprite );
		}

		const sol::optional<sol::table> luaBoxCollider = ( *components )[ "boxCollider" ];
		if ( luaBoxCollider )
		{
			auto& boxCollider = gameObject.AddComponent<BoxColliderComponent>();
			DESERIALIZE_COMPONENT( *luaBoxCollider, boxCollider );
		}

		const sol::optional<sol::table> luaCircleCollider = ( *components )[ "circleCollider" ];
		if ( luaCircleCollider )
		{
			auto& circleCollider = gameObject.AddComponent<CircleColliderComponent>();
			DESERIALIZE_COMPONENT( *luaCircleCollider, circleCollider );
		}

		const sol::optional<sol::table> luaAnimation = ( *components )[ "animation" ];
		if ( luaAnimation )
		{
			auto& animation = gameObject.AddComponent<AnimationComponent>();
			DESERIALIZE_COMPONENT( *luaAnimation, animation );
		}

		const sol::optional<sol::table> luaPhysics = ( *components )[ "physics" ];
		if ( luaPhysics )
		{
			auto& physics = gameObject.AddComponent<PhysicsComponent>();
			DESERIALIZE_COMPONENT( *luaPhysics, physics );
		}

		const sol::optional<sol::table> luaID = ( *components )[ "id" ];
		if ( luaID )
		{
			auto& id = gameObject.GetComponent<Identification>();
			DESERIALIZE_COMPONENT( *luaID, id );
		}

		const sol::optional<sol::table> luaUI = ( *components )[ "ui" ];
		if ( luaUI )
		{
			auto& ui = gameObject.AddComponent<UIComponent>();
			DESERIALIZE_COMPONENT( *luaUI, ui );
		}

		const sol::optional<sol::table> luaRelations = ( *components )[ "relationship" ];
		if ( luaRelations )
		{
			SaveRelationship saveRelations{};
			saveRelations.sParent = ( *luaRelations )[ "parent" ].get_or( std::string{ "" } );
			saveRelations.sNextSibling = ( *luaRelations )[ "nextSibling" ].get_or( std::string{ "" } );
			saveRelations.sPrevSibling = ( *luaRelations )[ "prevSibling" ].get_or( std::string{ "" } );
			saveRelations.sFirstChild = ( *luaRelations )[ "firstChild" ].get_or( std::string{ "" } );

			mapEntityToRelationship.emplace( gameObject.GetEntity(), saveRelations );
		}
	}

	auto ids = registry.GetRegistry().view<Identification>( entt::exclude<TileComponent> );

	auto findTag = [ & ]( const std::string& sTag ) {
		auto parItr = std::ranges::find_if( ids, [ & ]( const auto& e ) {
			Entity en{ registry, e };
			return en.GetName() == sTag;
		} );

		if ( parItr != ids.end() )
		{
			return *parItr;
		}

		return entt::entity{ entt::null };
	};

	for ( auto& [ entity, saveRelations ] : mapEntityToRelationship )
	{
		Entity ent{ registry, entity };
		auto& relations = ent.GetComponent<Relationship>();

		// Find the parent
		if ( !saveRelations.sParent.empty() )
		{
			relations.parent = findTag( saveRelations.sParent );
		}

		// Find the nextSibling
		if ( !saveRelations.sNextSibling.empty() )
		{
			relations.nextSibling = findTag( saveRelations.sNextSibling );
		}

		// Find the prevSibling
		if ( !saveRelations.sPrevSibling.empty() )
		{
			relations.prevSibling = findTag( saveRelations.sPrevSibling );
		}

		// Find the firstChild
		if ( !saveRelations.sFirstChild.empty() )
		{
			relations.firstChild = findTag( saveRelations.sFirstChild );
		}
	}

	return true;
}

bool TilemapLoader::SaveTilemap( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON )
{
	return bUseJSON ? SaveTilemapJSON( registry, sTilemapFile ) : SaveTilemapLua( registry, sTilemapFile );
}

bool TilemapLoader::LoadTilemap( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON )
{
	return bUseJSON ? LoadTilemapJSON( registry, sTilemapFile ) : LoadTilemapLua( registry, sTilemapFile );
}

bool TilemapLoader::LoadGameObjects( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile,
									 bool bUseJSON )
{
	return bUseJSON ? LoadObjectMapJSON( registry, sObjectMapFile ) : LoadObjectMapLua( registry, sObjectMapFile );
}

bool TilemapLoader::SaveGameObjects( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile,
									 bool bUseJSON )
{
	return bUseJSON ? SaveObjectMapJSON( registry, sObjectMapFile ) : SaveObjectMapLua( registry, sObjectMapFile );
}

bool TilemapLoader::LoadTilemapFromLuaTable( SCION_CORE::ECS::Registry& registry, const sol::table& sTilemapTable )
{
	if ( !sTilemapTable.valid() || sTilemapTable.get_type() != sol::type::table )
	{
		SCION_ERROR( "Failed to load tilemap map from table. Table is invalid." );
		return false;
	}

	sol::optional<sol::table> maybeTiles = sTilemapTable[ "tilemap" ];
	if ( !maybeTiles )
	{
		SCION_ERROR( "Failed to load tilemap map file: \"tilemap\" table is missing or invalid." );
		return false;
	}

	for ( const auto& [ key, value ] : *maybeTiles )
	{
		Entity newTile{ registry, "", "" };
		const sol::optional<sol::table> components = value.as<sol::table>()[ "components" ];

		if ( !components )
		{
			SCION_ERROR( "Failed to load object map file: \"components\" table is missing or invalid." );
			return false;
		}

		// Transform
		const sol::table luaTransform = ( *components )[ "transform" ];
		auto& transform = newTile.AddComponent<TransformComponent>();
		DESERIALIZE_COMPONENT( luaTransform, transform );

		// Sprite
		const sol::table luaSprite = ( *components )[ "sprite" ];
		auto& sprite = newTile.AddComponent<SpriteComponent>();
		DESERIALIZE_COMPONENT( luaSprite, sprite );

		sol::optional<sol::table> luaBoxCollider = ( *components )[ "boxCollider" ];
		if ( luaBoxCollider )
		{
			auto& boxCollider = newTile.AddComponent<BoxColliderComponent>();
			DESERIALIZE_COMPONENT( *luaBoxCollider, boxCollider );
		}

		sol::optional<sol::table> luaCircleCollider = ( *components )[ "circleCollider" ];
		if ( luaCircleCollider )
		{
			auto& circleCollider = newTile.AddComponent<CircleColliderComponent>();
			DESERIALIZE_COMPONENT( *luaCircleCollider, circleCollider );
		}

		sol::optional<sol::table> luaAnimations = ( *components )[ "animation" ];
		if ( luaAnimations )
		{
			auto& animation = newTile.AddComponent<AnimationComponent>();
			DESERIALIZE_COMPONENT( *luaAnimations, animation );
		}

		sol::optional<sol::table> luaPhysics = ( *components )[ "physics" ];
		if ( luaPhysics )
		{
			auto& physics = newTile.AddComponent<PhysicsComponent>();
			DESERIALIZE_COMPONENT( *luaPhysics, physics );
		}

		newTile.AddComponent<TileComponent>( TileComponent{ .id = static_cast<uint32_t>( newTile.GetEntity() ) } );
	}

	return true;
}

bool TilemapLoader::LoadGameObjectsFromLuaTable( SCION_CORE::ECS::Registry& registry, const sol::table& sObjectTable )
{
	if ( !sObjectTable.valid() || sObjectTable.get_type() != sol::type::table )
	{
		SCION_ERROR( "Failed to load object map from table. Table is invalid." );
		return false;
	}

	sol::optional<sol::table> maybeObjects = sObjectTable[ "game_objects" ];
	if ( !maybeObjects )
	{
		SCION_ERROR( "Failed to load objects map file: \"game_objects\" table is missing or invalid." );
		return false;
	}

	for ( const auto& [ key, value ] : *maybeObjects )
	{
		Entity newTile{ registry, "", "" };
		const sol::optional<sol::table> components = value.as<sol::table>()[ "components" ];

		if ( !components )
		{
			SCION_ERROR( "Failed to load object map file: \"components\" table is missing or invalid." );
			return false;
		}

		// Transform
		const sol::table luaTransform = ( *components )[ "transform" ];
		auto& transform = newTile.AddComponent<TransformComponent>();
		DESERIALIZE_COMPONENT( luaTransform, transform );

		// Sprite
		sol::optional<sol::table> optLuaSprite = ( *components )[ "sprite" ];
		if ( optLuaSprite )
		{
			auto& sprite = newTile.AddComponent<SpriteComponent>();
			DESERIALIZE_COMPONENT( *optLuaSprite, sprite );
		}

		sol::optional<sol::table> luaID = ( *components )[ "id" ];
		if ( luaID )
		{
			auto& id = newTile.GetComponent<Identification>();
			DESERIALIZE_COMPONENT( *luaID, id );
			newTile.ChangeName( id.name );
		}

		sol::optional<sol::table> luaBoxCollider = ( *components )[ "boxCollider" ];
		if ( luaBoxCollider )
		{
			auto& boxCollider = newTile.AddComponent<BoxColliderComponent>();
			DESERIALIZE_COMPONENT( *luaBoxCollider, boxCollider );
		}

		sol::optional<sol::table> luaCircleCollider = ( *components )[ "circleCollider" ];
		if ( luaCircleCollider )
		{
			auto& circleCollider = newTile.AddComponent<CircleColliderComponent>();
			DESERIALIZE_COMPONENT( *luaCircleCollider, circleCollider );
		}

		sol::optional<sol::table> luaAnimations = ( *components )[ "animation" ];
		if ( luaAnimations )
		{
			auto& animation = newTile.AddComponent<AnimationComponent>();
			DESERIALIZE_COMPONENT( *luaAnimations, animation );
		}

		sol::optional<sol::table> luaPhysics = ( *components )[ "physics" ];
		if ( luaPhysics )
		{
			auto& physics = newTile.AddComponent<PhysicsComponent>();
			DESERIALIZE_COMPONENT( *luaPhysics, physics );
		}

		sol::optional<sol::table> optLuaText = ( *components )[ "text" ];
		if ( optLuaText )
		{
			auto& text = newTile.AddComponent<TextComponent>();
			DESERIALIZE_COMPONENT( *optLuaText, text );
		}

		sol::optional<sol::table> optUI = ( *components )[ "ui" ];
		if ( optUI )
		{
			auto& ui = newTile.AddComponent<UIComponent>();
			DESERIALIZE_COMPONENT( *optUI, ui );
		}
	}

	return true;
}


} // namespace SCION_CORE::Loaders
