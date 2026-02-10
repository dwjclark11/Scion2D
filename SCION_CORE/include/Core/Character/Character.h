#pragma once
#include "Core/ECS/Entity.h"

namespace Scion::Core::ECS
{
struct PhysicsAttributes;
struct TransformComponent;
struct SpriteComponent;
struct AnimationComponent;
struct BoxColliderComponent;
struct CircleColliderComponent;
} // namespace Scion::Core::ECS

namespace Scion::Core
{
class StateMachine;

struct CharacterParams
{
	std::string sName{};
	std::string sGroup{};

	std::optional<std::unique_ptr<Scion::Core::ECS::AnimationComponent>> animation{ std::nullopt };
	std::optional<std::unique_ptr<Scion::Core::ECS::SpriteComponent>> sprite{ std::nullopt };
	std::optional<std::unique_ptr<Scion::Core::ECS::BoxColliderComponent>> boxCollider{ std::nullopt };
	std::optional<std::unique_ptr<Scion::Core::ECS::CircleColliderComponent>> circleCollider{ std::nullopt };
	std::optional<std::unique_ptr<Scion::Core::ECS::PhysicsAttributes>> physicsParams{ std::nullopt };
};

class Character : public Scion::Core::ECS::Entity
{
  public:
	Character( Scion::Core::ECS::Registry& registry, const CharacterParams& params );
	Character( Scion::Core::ECS::Registry& registry, entt::entity entity );
	Character( const Scion::Core::ECS::Entity& entity );
	~Character();

	StateMachine& GetStateMachine();

	Scion::Core::ECS::TransformComponent& GetTransformComponent();
	Scion::Core::ECS::SpriteComponent& GetSpriteComponent();
	Scion::Core::ECS::AnimationComponent& GetAnimationComponent();

	static void CreateCharacterLuaBind( sol::state& lua, Scion::Core::ECS::Registry& registry );

  protected:
	std::shared_ptr<StateMachine> m_pStateMachine;

	// TODO: Add whatever else might be needed for characters!
};
} // namespace Scion::Core
