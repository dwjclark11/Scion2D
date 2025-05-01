#pragma once
#include "Core/ECS/Entity.h"
#include <optional>

namespace SCION_CORE::ECS
{
struct PhysicsAttributes;
struct TransformComponent;
struct SpriteComponent;
struct AnimationComponent;
struct BoxColliderComponent;
struct CircleColliderComponent;
} // namespace SCION_CORE::ECS

namespace SCION_CORE
{
class StateMachine;

struct CharacterParams
{
	std::string sName{ "" };
	std::string sGroup{ "" };
	
	std::optional<std::unique_ptr<SCION_CORE::ECS::AnimationComponent>> animation{ std::nullopt };
	std::optional<std::unique_ptr<SCION_CORE::ECS::SpriteComponent>> sprite{ std::nullopt };
	std::optional<std::unique_ptr<SCION_CORE::ECS::BoxColliderComponent>> boxCollider{ std::nullopt };
	std::optional<std::unique_ptr<SCION_CORE::ECS::CircleColliderComponent>> circleCollider{ std::nullopt };
	std::optional<std::unique_ptr<SCION_CORE::ECS::PhysicsAttributes>> physicsParams{ std::nullopt };
};

class Character : public SCION_CORE::ECS::Entity
{
  public:
	Character( SCION_CORE::ECS::Registry& registry, const CharacterParams& params );
	Character( SCION_CORE::ECS::Registry& registry, entt::entity entity );
	Character( const SCION_CORE::ECS::Entity& entity );
	~Character();

	StateMachine& GetStateMachine();

	SCION_CORE::ECS::TransformComponent& GetTransformComponent();
	SCION_CORE::ECS::SpriteComponent& GetSpriteComponent();
	SCION_CORE::ECS::AnimationComponent& GetAnimationComponent();

	static void CreateCharacterLuaBind( sol::state& lua, SCION_CORE::ECS::Registry& registry );

  protected:
	std::shared_ptr<StateMachine> m_pStateMachine;

	// TODO: Add whatever else might be needed for characters!
};
} // namespace SCION_CORE
