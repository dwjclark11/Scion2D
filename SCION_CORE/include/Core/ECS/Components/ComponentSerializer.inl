#include "ComponentSerializer.h"

namespace SCION_CORE::ECS
{
template <typename TComponent, typename TSerializer>
inline void ComponentSerializer::Serialize( TSerializer& serializer, const TComponent& component )
{
	SerializeComponent( serializer, component );
}

template <typename TComponent, typename TTable>
inline void ComponentSerializer::Deserialize( const TTable& table, TComponent& component )
{
	DeserializeComponent( table, component );
}

template <typename TComponent, typename TTable>
inline auto ComponentSerializer::Deserialize( const TTable& table )
{
	if constexpr ( std::is_same_v<TComponent, TransformComponent> )
	{
		return DeserializeTransform( table );
	}
	else if constexpr ( std::is_same_v<TComponent, SpriteComponent> )
	{
		return DeserializeSprite( table );
	}
	else if constexpr ( std::is_same_v<TComponent, AnimationComponent> )
	{
		return DeserializeAnimation( table );
	}
	else if constexpr ( std::is_same_v<TComponent, BoxColliderComponent> )
	{
		return DeserializeBoxCollider( table );
	}
	else if constexpr ( std::is_same_v<TComponent, CircleColliderComponent> )
	{
		return DeserializeCircleCollider( table );
	}
	else if constexpr ( std::is_same_v<TComponent, TextComponent> )
	{
		return DeserializeTextComponent( table );
	}
	else if constexpr ( std::is_same_v<TComponent, PhysicsComponent> )
	{
		return DeserializePhysics( table );
	}
	else if constexpr ( std::is_same_v<TComponent, RigidBodyComponent> )
	{
		return DeserializeRigidBody( table );
	}
	else
	{
		static_assert( false, "Component type is invalid!" );
	}
}

}
