#pragma once
#include "AllComponents.h"
#include <rapidjson/document.h>
#include <sol/sol.hpp>

namespace Scion::Filesystem
{
class JSONSerializer;
class LuaSerializer;
}

#define SERIALIZE_COMPONENT( serializer, component )                                                                   \
	Scion::Core::ECS::ComponentSerializer::Serialize( serializer, component )

#define DESERIALIZE_COMPONENT( table, compref ) Scion::Core::ECS::ComponentSerializer::Deserialize( table, compref )

namespace Scion::Core::ECS
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
	static void SerializeComponent( Scion::Filesystem::JSONSerializer& serializer, const TransformComponent& transform );
	static void SerializeComponent( Scion::Filesystem::JSONSerializer& serializer, const SpriteComponent& sprite );
	static void SerializeComponent( Scion::Filesystem::JSONSerializer& serializer, const AnimationComponent& animation );
	static void SerializeComponent( Scion::Filesystem::JSONSerializer& serializer,
									const BoxColliderComponent& boxCollider );
	static void SerializeComponent( Scion::Filesystem::JSONSerializer& serializer,
									const CircleColliderComponent& circleCollider );
	static void SerializeComponent( Scion::Filesystem::JSONSerializer& serializer, const TextComponent& text );
	static void SerializeComponent( Scion::Filesystem::JSONSerializer& serializer, const PhysicsComponent& physics );
	static void SerializeComponent( Scion::Filesystem::JSONSerializer& serializer, const RigidBodyComponent& rigidBody );
	static void SerializeComponent( Scion::Filesystem::JSONSerializer& serializer, const Identification& id);
	static void SerializeComponent( Scion::Filesystem::JSONSerializer& serializer, const UIComponent& id);

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
	static void SerializeComponent( Scion::Filesystem::LuaSerializer& serializer, const TransformComponent& transform );
	static void SerializeComponent( Scion::Filesystem::LuaSerializer& serializer, const SpriteComponent& sprite );
	static void SerializeComponent( Scion::Filesystem::LuaSerializer& serializer, const AnimationComponent& animation );
	static void SerializeComponent( Scion::Filesystem::LuaSerializer& serializer,
									const BoxColliderComponent& boxCollider );
	static void SerializeComponent( Scion::Filesystem::LuaSerializer& serializer,
									const CircleColliderComponent& circleCollider );
	static void SerializeComponent( Scion::Filesystem::LuaSerializer& serializer, const TextComponent& text );
	static void SerializeComponent( Scion::Filesystem::LuaSerializer& serializer, const PhysicsComponent& physics );
	static void SerializeComponent( Scion::Filesystem::LuaSerializer& serializer, const RigidBodyComponent& rigidBody );
	static void SerializeComponent( Scion::Filesystem::LuaSerializer& serializer, const Identification& id );
	static void SerializeComponent( Scion::Filesystem::LuaSerializer& serializer, const UIComponent& ui );

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

} // namespace Scion::Core::ECS

#include "ComponentSerializer.inl"
