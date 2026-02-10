#pragma once
#include "Core/ECS/Components/AllComponents.h"

namespace Scion::Core::ECS
{
class Entity;
}

namespace Scion::Editor
{
class DrawComponentsUtil
{
  public:
	DrawComponentsUtil() = delete;

	template <typename TComponent>
	static void DrawEntityComponentInfo( Scion::Core::ECS::Entity& entity );

	template <typename TComponent>
	static void DrawComponentInfo( TComponent& component );

	template <typename TComponent>
	static void DrawComponentInfo( Scion::Core::ECS::Entity& entity, TComponent& component );

	template <typename TComponent>
	static void RegisterUIComponent();

  private:
	static void DrawImGuiComponent( Scion::Core::ECS::TransformComponent& transform );
	static void DrawImGuiComponent( Scion::Core::ECS::SpriteComponent& sprite );
	static void DrawImGuiComponent( Scion::Core::ECS::AnimationComponent& animation );
	static void DrawImGuiComponent( Scion::Core::ECS::BoxColliderComponent& boxCollider );
	static void DrawImGuiComponent( Scion::Core::ECS::CircleColliderComponent& circleCollider );
	static void DrawImGuiComponent( Scion::Core::ECS::PhysicsComponent& physics );
	static void DrawImGuiComponent( Scion::Core::ECS::RigidBodyComponent& rigidbody );
	static void DrawImGuiComponent( Scion::Core::ECS::TextComponent& textComponent );
	static void DrawImGuiComponent( Scion::Core::ECS::Identification& identification );

	// Test to deal with Relationships.
	static void DrawImGuiComponent( Scion::Core::ECS::Entity& entity, Scion::Core::ECS::TransformComponent& transform );
	static void DrawImGuiComponent( Scion::Core::ECS::Entity& entity, Scion::Core::ECS::SpriteComponent& sprite );
	static void DrawImGuiComponent( Scion::Core::ECS::Entity& entity, Scion::Core::ECS::AnimationComponent& animation );
	static void DrawImGuiComponent( Scion::Core::ECS::Entity& entity,
									Scion::Core::ECS::BoxColliderComponent& boxCollider );
	static void DrawImGuiComponent( Scion::Core::ECS::Entity& entity,
									Scion::Core::ECS::CircleColliderComponent& circleCollider );
	static void DrawImGuiComponent( Scion::Core::ECS::Entity& entity, Scion::Core::ECS::PhysicsComponent& physics );
	static void DrawImGuiComponent( Scion::Core::ECS::Entity& entity, Scion::Core::ECS::RigidBodyComponent& rigidbody );
	static void DrawImGuiComponent( Scion::Core::ECS::Entity& entity, Scion::Core::ECS::TextComponent& textComponent );
	static void DrawImGuiComponent( Scion::Core::ECS::Entity& entity, Scion::Core::ECS::Identification& identification );
};

} // namespace Scion::Editor

#include "DrawComponentUtils.inl"
