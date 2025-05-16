#include "Core/Character/Character.h"
#include "Core/States/StateMachine.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MetaUtilities.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"

#include "Logger/Logger.h"

using namespace SCION_CORE::Utils;
using namespace SCION_CORE::ECS;
using namespace entt::literals;

namespace SCION_CORE
{

Character::Character( SCION_CORE::ECS::Registry& registry, const CharacterParams& params )
	: Entity{ registry, params.sName, params.sGroup }
{
	AddComponent<TransformComponent>( TransformComponent{} );

	// Check to see if the components have been set for the params. If set, add components
	// with those values, if not, add empty components.
	if ( params.sprite )
	{
		auto& sprite = AddComponent<SpriteComponent>( *params.sprite.value() );
		auto pTexture = ASSET_MANAGER().GetTexture( sprite.sTextureName );
		if ( pTexture )
		{
			GenerateUVs( sprite, pTexture->GetWidth(), pTexture->GetHeight() );
		}
		else
		{
			SCION_ERROR( "Failed to generate sprite UVs - Texture [{}] is invalid.", sprite.sTextureName );
		}
	}
	else
	{
		AddComponent<SpriteComponent>( SpriteComponent{} );
	}

	if ( params.animation )
	{
		AddComponent<AnimationComponent>( *params.animation.value() );
	}
	else
	{
		AddComponent<AnimationComponent>( AnimationComponent{} );
	}

	// Only add a box collider if it is explicitly set.
	// Circle colliders are done by default.
	if ( params.boxCollider )
	{
		AddComponent<BoxColliderComponent>( *params.boxCollider.value() );
	}

	// If there is a box collider, do not add a circle collider
	if ( params.circleCollider && !params.boxCollider )
	{
		AddComponent<CircleColliderComponent>( *params.circleCollider.value() );
	}
	else if ( !params.boxCollider )
	{
		AddComponent<CircleColliderComponent>( CircleColliderComponent{} );
	}

	// Only add a physics component if the physics params are set.
	if ( params.physicsParams )
	{
		AddComponent<PhysicsComponent>( *params.physicsParams.value() );
	}
}

Character::Character( SCION_CORE::ECS::Registry& registry, entt::entity entity )
	: Entity{ registry, entity }
{
	// TODO: verify that it has all default components
}

Character::Character( const SCION_CORE::ECS::Entity& entity )
	: Entity{ entity }
{
}

Character::~Character()
{

}

StateMachine& Character::GetStateMachine()
{
	if ( !m_pStateMachine )
		m_pStateMachine = std::make_shared<StateMachine>();

	SCION_ASSERT( m_pStateMachine && "State Machine must be valid." );
	return *m_pStateMachine;
}

SCION_CORE::ECS::TransformComponent& Character::GetTransformComponent()
{
	return GetComponent<TransformComponent>();
}

SCION_CORE::ECS::SpriteComponent& Character::GetSpriteComponent()
{
	return GetComponent<SpriteComponent>();
}

SCION_CORE::ECS::AnimationComponent& Character::GetAnimationComponent()
{
	return GetComponent<AnimationComponent>();
}

void Character::CreateCharacterLuaBind( sol::state& lua, SCION_CORE::ECS::Registry& registry )
{
	lua.new_usertype<CharacterParams>(
		"CharacterParams",
		"type_id",
		entt::type_hash<CharacterParams>::value,
		sol::call_constructor,
		sol::factories(
			[]( const std::string& sName,
				const std::string& sGroup,
				const SpriteComponent& sprite,
				const AnimationComponent& animation,
				const BoxColliderComponent& boxCollider,
				const PhysicsAttributes& attr ) {
				return CharacterParams{ .sName = sName,
										.sGroup = sGroup,
										.animation = std::make_unique<AnimationComponent>( animation ),
										.sprite = std::make_unique<SpriteComponent>( sprite ),
										.boxCollider = std::make_unique<BoxColliderComponent>( boxCollider ),
										.physicsParams = std::make_unique<PhysicsAttributes>( attr ) };
			},
			[]( const std::string& sName,
				const std::string& sGroup,
				const SpriteComponent& sprite,
				const AnimationComponent& animation,
				const CircleColliderComponent& circleCollider,
				const PhysicsAttributes& attr ) {
				return CharacterParams{ .sName = sName,
										.sGroup = sGroup,
										.animation = std::make_unique<AnimationComponent>( animation ),
										.sprite = std::make_unique<SpriteComponent>( sprite ),
										.circleCollider = std::make_unique<CircleColliderComponent>( circleCollider ),
										.physicsParams = std::make_unique<PhysicsAttributes>( attr ) };
			},
			[]( const std::string& sName,
				const std::string& sGroup,
				const SpriteComponent& sprite,
				const AnimationComponent& animation,
				const BoxColliderComponent& boxCollider ) {
				return CharacterParams{ .sName = sName,
										.sGroup = sGroup,
										.animation = std::make_unique<AnimationComponent>( animation ),
										.sprite = std::make_unique<SpriteComponent>( sprite ),
										.boxCollider = std::make_unique<BoxColliderComponent>( boxCollider ) };
			},
			[]( const std::string& sName,
				const std::string& sGroup,
				const SpriteComponent& sprite,
				const AnimationComponent& animation,
				const CircleColliderComponent& circleCollider ) {
				return CharacterParams{ .sName = sName,
										.sGroup = sGroup,
										.animation = std::make_unique<AnimationComponent>( animation ),
										.sprite = std::make_unique<SpriteComponent>( sprite ),
										.circleCollider = std::make_unique<CircleColliderComponent>( circleCollider ) };
			},
			[]( const std::string& sName, const std::string& sGroup ) {
				return CharacterParams{ .sName = sName, .sGroup = sGroup };
			} ) );

	lua.new_usertype<Character>(
		"Character",
		"type_id",
		entt::type_hash<Character>::value,
		sol::call_constructor,
		sol::factories( [ & ]( const CharacterParams& params ) { return Character{ registry, params }; },
						[ & ]( std::uint32_t id ) { return Character{ registry, static_cast<entt::entity>( id ) }; } ),
		"addComponent",
		[]( Character& character, const sol::table& comp, sol::this_state s ) -> sol::object {
			if ( !comp.valid() )
				return sol::lua_nil_t{};

			const auto component = InvokeMetaFunction( GetIdType( comp ), "add_component"_hs, static_cast<Entity&>(character), comp, s );

			return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"hasComponent",
		[]( Character& character, const sol::table& comp ) {
			const auto has_comp = InvokeMetaFunction( GetIdType( comp ), "has_component"_hs, static_cast<Entity&>(character) );

			return has_comp ? has_comp.cast<bool>() : false;
		},
		"getComponent",
		[]( Character& character, const sol::table& comp, sol::this_state s ) {
			const auto component = InvokeMetaFunction( GetIdType( comp ), "get_component"_hs, static_cast<Entity&>(character), s );

			return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"removeComponent",
		[]( Character& character, const sol::table& comp ) {
			const auto component = InvokeMetaFunction( GetIdType( comp ), "remove_component"_hs, static_cast<Entity&>(character) );

			return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"getSprite",
		[]( Character& character, sol::this_state s ) {
			return sol::make_reference( s, std::ref( character.GetSpriteComponent() ) );
		},
		"getTransform",
		[]( Character& character, sol::this_state s ) {
			return sol::make_reference( s, std::ref( character.GetTransformComponent() ) );
		},
		"getAnimation",
		[]( Character& character, sol::this_state s ) {
			return sol::make_reference( s, std::ref( character.GetAnimationComponent() ) );
		},
		"name",
		&Character::GetName,
		"group",
		&Character::GetGroup,
		"kill",
		&Character::Kill,
		"addChild",
		[]( Character& character, Entity& child ) { character.AddChild( child.GetEntity() ); },
		"updateTransform",
		&Character::UpdateTransform,
		"id",
		[]( Character& character ) { return static_cast<uint32_t>( character.GetEntity() ); },
		"getStateMachine",
		[]( Character& character ) { return character.GetStateMachine(); }

	);
}

} // namespace SCION_CORE
