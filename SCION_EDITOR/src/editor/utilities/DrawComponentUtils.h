#pragma once
#include "Core/ECS/Components/AllComponents.h"

namespace SCION_CORE::ECS
{
class Entity;
}

namespace SCION_EDITOR
{
class DrawComponentsUtil
{
  public:
	DrawComponentsUtil() = delete;

	template <typename TComponent>
	static void DrawEntityComponentInfo( SCION_CORE::ECS::Entity& entity );

	template <typename TComponent>
	static void DrawComponentInfo( TComponent& component );

	template <typename TComponent>
	static void RegisterUIComponent();

  private:
	static void DrawImGuiComponent( SCION_CORE::ECS::TransformComponent& transform );
	static void DrawImGuiComponent( SCION_CORE::ECS::SpriteComponent& sprite );
	static void DrawImGuiComponent( SCION_CORE::ECS::AnimationComponent& animation);
	static void DrawImGuiComponent( SCION_CORE::ECS::BoxColliderComponent& boxCollider);
	static void DrawImGuiComponent( SCION_CORE::ECS::CircleColliderComponent& circleCollider);
	static void DrawImGuiComponent( SCION_CORE::ECS::PhysicsComponent& physics);
	static void DrawImGuiComponent( SCION_CORE::ECS::RigidBodyComponent& rigidbody);
	static void DrawImGuiComponent( SCION_CORE::ECS::TextComponent& textComponent);
	static void DrawImGuiComponent( SCION_CORE::ECS::Identification& identification);
};

} // namespace SCION_EDITOR

#include "DrawComponentUtils.inl"
