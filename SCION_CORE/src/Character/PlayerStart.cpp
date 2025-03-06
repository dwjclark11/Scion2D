#include "Core/Character/PlayerStart.h"
#include "Core/Character/Character.h"
#include "Core/CoreUtilities/Prefab.h"
#include "Core/Scene/Scene.h"

#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"

#include "Core/CoreUtilities/CoreUtilities.h"

using namespace SCION_CORE::ECS;

constexpr const char* PlayerStartTag = "S2D_PlayerStart";

namespace SCION_CORE
{

PlayerStart::PlayerStart( SCION_CORE::ECS::Registry& registry, Scene& sceneRef )
	: m_SceneRef{ sceneRef }
	, m_VisualEntity{ registry, PlayerStartTag, "" }
	, m_pCharacterPrefab{ nullptr }
	, m_pCharacter{ nullptr }
	, m_sCharacterName{ "default" }
	, m_bCharacterLoaded{ false }
	, m_bVisualEntityCreated{ false }
{
	LoadVisualEntity();
}

void PlayerStart::CreatePlayer( SCION_CORE::ECS::Registry& registry )
{
	if ( m_sCharacterName != "default" && !m_pCharacterPrefab && m_bCharacterLoaded )
	{
		if ( auto pPrefab = ASSET_MANAGER().GetPrefab( m_sCharacterName ) )
		{
			SetCharacter( *pPrefab );
			auto pNewEntity = PrefabCreator::AddPrefabToScene( *m_pCharacterPrefab, registry );
			auto& transform = pNewEntity->GetComponent<TransformComponent>();
			const auto& playerStartTransform = m_VisualEntity.GetComponent<TransformComponent>();
			transform.position = playerStartTransform.position;
		}
		else
		{
			SCION_ERROR( "Failed to create player start character. [{}] prefabbed character does not exist.",
						 m_sCharacterName );
			return;
		}
	}
	else if ( m_pCharacterPrefab )
	{
		auto pNewEntity = PrefabCreator::AddPrefabToScene( *m_pCharacterPrefab, registry );
		auto& transform = pNewEntity->GetComponent<TransformComponent>();
		const auto& playerStartTransform = m_VisualEntity.GetComponent<TransformComponent>();
		transform.position = playerStartTransform.position;
	}
	else
	{
		/* DEFAULT PLAYER START IS NOT SET UP - PLEASE ENSURE THAT THE USER HAS SET A PREFAB TO USE! */
		// Create a default character.
		// TODO: Determine how we check the physics component.
		//	- If Iso metric, we always set the gravity to zero
		//	- If a platformer - Set gravity to 1,
		//	- If the game is a top down game - Set gravity to zero
		//  - If Physics is not enabled, do not add a physics component

		// Entity characterEnt{ registry, "Player", "" };
		// auto& transform =
		//	characterEnt.AddComponent<TransformComponent>( m_VisualEntity.GetComponent<TransformComponent>() );
		// transform.scale = glm::vec2{ 2.f };
		//// This needs to be a default texture in the engine.
		//// We should have a couple of different ones based on the type of game we want to make
		// auto& sprite = characterEnt.AddComponent<SpriteComponent>(
		//	SpriteComponent{ .sTextureName = "player_sprite", .width = 32, .height = 32, .layer = 6 } );

		// sprite.bIsoMetric = m_SceneRef.GetMapType() == EMapType::IsoGrid;

		// auto pTexture = ASSET_MANAGER().GetTexture( "player_sprite" );
		// SCION_ASSERT( pTexture && "The default player texture must exist." );

		// GenerateUVs( sprite, pTexture->GetWidth(), pTexture->GetHeight() );

		// characterEnt.AddComponent<CircleColliderComponent>(
		//	CircleColliderComponent{ .radius = 10, .offset = glm::vec2{ 10.f, 12.f } } );

		// characterEnt.AddComponent<PhysicsComponent>(
		//	PhysicsComponent{ PhysicsAttributes{ .eType = SCION_PHYSICS::RigidBodyType::DYNAMIC,
		//										 .density = 100.f,
		//										 .friction = 0.f,
		//										 .restitution = 0.f,
		//										 .radius = 10,
		//										 .gravityScale = 0.f,
		//										 .bCircle = true } } );

		// characterEnt.AddComponent<AnimationComponent>(
		//	AnimationComponent{ .numFrames = 6, .frameRate = 10, .bLooped = true } );
	}
}

std::string PlayerStart::GetCharacterName()
{
	if ( !m_sCharacterName.empty() && m_bCharacterLoaded )
		return m_sCharacterName;

	return std::string{ "default" };
}

void PlayerStart::SetCharacter( const Prefab& prefab )
{
	if ( m_pCharacterPrefab )
		m_pCharacterPrefab.reset();

	m_pCharacterPrefab = std::make_shared<Prefab>( prefab );

	if ( auto& id = m_pCharacterPrefab->GetPrefabbedEntity().id )
	{
		m_sCharacterName = id->name;
	}

	m_bCharacterLoaded = true;
}

glm::vec2 PlayerStart::GetPosition()
{
	const auto& transform = m_VisualEntity.GetComponent<TransformComponent>();
	return transform.position;
}

void PlayerStart::SetPosition( const glm::vec2& position )
{
	auto* transform = m_VisualEntity.TryGetComponent<TransformComponent>();
	SCION_ASSERT( transform && "Visual entity was not setup correctly" );
	transform->position = position;
}

void PlayerStart::Load( const std::string& sPrefabName )
{
	m_sCharacterName = sPrefabName;
	m_bCharacterLoaded = true;
}

void PlayerStart::Unload()
{
	// We want to reset the entity to entt::null
	m_VisualEntity.GetEntity() = entt::null;
	m_bVisualEntityCreated = false;

	//m_pCharacter.reset( );
	//m_pCharacterPrefab.reset( );
	//m_bCharacterLoaded = false;
	//m_sCharacterName.clear();
}

void PlayerStart::LoadVisualEntity()
{
	if (m_bVisualEntityCreated)
	{
		SCION_ERROR( "Failed to load visual entity. Already created." );
		return;
	}

	if (m_VisualEntity.GetEntity() == entt::null)
	{
		m_VisualEntity = Entity{ m_SceneRef.GetRegistry(), PlayerStartTag, "" };
	}

	m_VisualEntity.AddComponent<TransformComponent>( TransformComponent{} );
	m_VisualEntity.AddComponent<UneditableComponent>( UneditableComponent{ .eType = EUneditableType::PlayerStart } );
	auto& sprite = m_VisualEntity.AddComponent<SpriteComponent>(
		SpriteComponent{ .sTextureName = "ZZ_S2D_PlayerStart", .width = 64, .height = 64, .layer = 999999 } );

	auto pTexture = MAIN_REGISTRY().GetAssetManager().GetTexture( sprite.sTextureName );
	SCION_ASSERT( pTexture && "ZZ_S2D_PlayerStart texture must be loaded into the asset manager!" );

	GenerateUVs( sprite, pTexture->GetWidth(), pTexture->GetHeight() );

	m_bVisualEntityCreated = true;
}

} // namespace SCION_CORE
