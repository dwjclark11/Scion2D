#pragma once
#include "AllComponents.h"
#include <rapidjson/document.h>
#include <sol/sol.hpp>

namespace SCION_FILESYSTEM
{
class JSONSerializer;
class LuaSerializer;
}

#define SERIALIZE_COMPONENT( serializer, component )                                                                   \
	SCION_CORE::ECS::ComponentSerializer::Serialize( serializer, component )

#define DESERIALIZE_COMPONENT( table, compref ) SCION_CORE::ECS::ComponentSerializer::Deserialize( table, compref )

namespace SCION_CORE::ECS
{
class ComponentSerializer
{
  public:
	ComponentSerializer() = delete;

	template <typename TComponent, typename TSerializer>
	static void Serialize( TSerializer& serializer, const TComponent& component );

	template <typename TComponent, typename TTable>
	static void Deserialize( const TTable& table, TComponent& component );

  private:
	// JSON serializer
	static void SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer, const TransformComponent& transform );
	static void SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer, const SpriteComponent& sprite );
	static void SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer, const AnimationComponent& animation );
	static void SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer,
									const BoxColliderComponent& boxCollider );
	static void SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer,
									const CircleColliderComponent& circleCollider );
	static void SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer, const TextComponent& text );
	static void SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer, const PhysicsComponent& physics );
	static void SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer, const RigidBodyComponent& rigidBody );
	static void SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer, const Identification& id);
	static void SerializeComponent( SCION_FILESYSTEM::JSONSerializer& serializer, const UIComponent& id);

	static void DeserializeComponent( const rapidjson::Value& jsonValue, TransformComponent& transform );
	static void DeserializeComponent( const rapidjson::Value& jsonValue, SpriteComponent& sprite );
	static void DeserializeComponent( const rapidjson::Value& jsonValue, AnimationComponent& animation );
	static void DeserializeComponent( const rapidjson::Value& jsonValue, BoxColliderComponent& boxCollider );
	static void DeserializeComponent( const rapidjson::Value& jsonValue, CircleColliderComponent& circleCollider );
	static void DeserializeComponent( const rapidjson::Value& jsonValue, TextComponent& text );
	static void DeserializeComponent( const rapidjson::Value& jsonValue, PhysicsComponent& physics );
	static void DeserializeComponent( const rapidjson::Value& jsonValue, RigidBodyComponent& rigidBody );
	static void DeserializeComponent( const rapidjson::Value& jsonValue, Identification& id);
	static void DeserializeComponent( const rapidjson::Value& jsonValue, UIComponent& id);

	// LUA serializer
	static void SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer, const TransformComponent& transform );
	static void SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer, const SpriteComponent& sprite );
	static void SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer, const AnimationComponent& animation );
	static void SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer,
									const BoxColliderComponent& boxCollider );
	static void SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer,
									const CircleColliderComponent& circleCollider );
	static void SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer, const TextComponent& text );
	static void SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer, const PhysicsComponent& physics );
	static void SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer, const RigidBodyComponent& rigidBody );
	static void SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer, const Identification& id );
	static void SerializeComponent( SCION_FILESYSTEM::LuaSerializer& serializer, const UIComponent& ui );

	static void DeserializeComponent( const sol::table& table, TransformComponent& transform );
	static void DeserializeComponent( const sol::table& table, SpriteComponent& sprite );
	static void DeserializeComponent( const sol::table& table, AnimationComponent& animation );
	static void DeserializeComponent( const sol::table& table, BoxColliderComponent& boxCollider );
	static void DeserializeComponent( const sol::table& table, CircleColliderComponent& circleCollider );
	static void DeserializeComponent( const sol::table& table, TextComponent& text );
	static void DeserializeComponent( const sol::table& table, PhysicsComponent& physics );
	static void DeserializeComponent( const sol::table& table, RigidBodyComponent& rigidBody );
	static void DeserializeComponent( const sol::table& table, Identification& id );
	static void DeserializeComponent( const sol::table& table, UIComponent& ui );
};

} // namespace SCION_CORE::ECS

#include "ComponentSerializer.inl"
