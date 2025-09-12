#include "Core/Character/PlayerStart.h"
#include "Core/Character/Character.h"
#include "Core/CoreUtilities/Prefab.h"
#include "Core/Scene/Scene.h"

#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/CoreUtilities/CoreEngineData.h"

#include <Rendering/Essentials/Texture.h>

using namespace SCION_CORE::ECS;

constexpr const std::string_view PlayerStartTag = "S2D_PlayerStart";

namespace SCION_CORE
{

PlayerStart::PlayerStart( SCION_CORE::ECS::Registry& registry, Scene& sceneRef )
	: m_SceneRef{ sceneRef }
	, m_pVisualEntity{ nullptr }
	, m_pCharacterPrefab{ nullptr }
	, m_pCharacter{ nullptr }
	, m_sCharacterName{ "default" }
	, m_bCharacterLoaded{ false }
	, m_bVisualEntityCreated{ false }
{
	
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
			const auto& playerStartTransform = m_pVisualEntity->GetComponent<TransformComponent>();
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
		const auto& playerStartTransform = m_pVisualEntity->GetComponent<TransformComponent>();
		transform.position = playerStartTransform.position;
	}
	else
	{
		Entity characterEnt{ &registry, "Player", "" };
		auto& transform =
			characterEnt.AddComponent<TransformComponent>( m_pVisualEntity->GetComponent<TransformComponent>() );
		transform.scale = glm::vec2{ 1.f }; // Should the scale be changed here?

		// This needs to be a default texture in the engine.
		// We should have a couple of different ones based on the type of game we want to make
		// Right now we just have a default player. Will add more later.
		auto& sprite = characterEnt.AddComponent<SpriteComponent>(
			SpriteComponent{ .sTextureName = "ZZ_S2D_default_player", .width = 16, .height = 16, .layer = 6 } );

		sprite.bIsoMetric = m_SceneRef.GetMapType() == EMapType::IsoGrid;

		auto pTexture = ASSET_MANAGER().GetTexture( "ZZ_S2D_default_player" );
		SCION_ASSERT( pTexture && "The default player texture must exist." );

		GenerateUVs( sprite, pTexture->GetWidth(), pTexture->GetHeight() );

		float radius{ 6.f };
		characterEnt.AddComponent<CircleColliderComponent>(
			CircleColliderComponent{ .radius = radius, .offset = glm::vec2{ 2.f, 2.f } } );

		auto& coreGlobals = CORE_GLOBALS();
		if ( coreGlobals.IsPhysicsEnabled() )
		{
			// If the game type is not a platformer, set to zero
			float gravityScale = coreGlobals.GetGameType() == EGameType::Platformer ? 1.f : 0.f;

			characterEnt.AddComponent<PhysicsComponent>(
				PhysicsComponent{ PhysicsAttributes{ .eType = SCION_PHYSICS::RigidBodyType::DYNAMIC,
													 .density = 100.f,
													 .friction = 0.f,
													 .restitution = 0.f,
													 .radius = radius,
													 .gravityScale = gravityScale,
													 .bCircle = true } } );
		}

		characterEnt.AddComponent<AnimationComponent>(
			AnimationComponent{ .numFrames = 4, .frameRate = 10, .bLooped = true } );
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
	const auto& transform = m_pVisualEntity->GetComponent<TransformComponent>();
	return transform.position;
}

void PlayerStart::SetPosition( const glm::vec2& position )
{
	auto* transform = m_pVisualEntity->TryGetComponent<TransformComponent>();
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
	if ( !m_bVisualEntityCreated )
		return;

	// We want to reset the entity to entt::null
	m_pVisualEntity->Destroy();
	m_pVisualEntity.reset();
	m_bVisualEntityCreated = false;

	// m_pCharacter.reset( );
	// m_pCharacterPrefab.reset( );
	// m_bCharacterLoaded = false;
	// m_sCharacterName.clear();
}

void PlayerStart::LoadVisualEntity()
{
	if ( m_bVisualEntityCreated )
	{
		SCION_ERROR( "Failed to load visual entity. Already created." );
		return;
	}

	if ( !m_pVisualEntity)
	{
		m_pVisualEntity =
			std::make_shared<Entity>( Entity{ m_SceneRef.GetRegistryPtr(), std::string{ PlayerStartTag }, "" } );
	}

	m_pVisualEntity->AddComponent<TransformComponent>( TransformComponent{} );
	m_pVisualEntity->AddComponent<UneditableComponent>( UneditableComponent{ .eType = EUneditableType::PlayerStart } );
	auto& sprite = m_pVisualEntity->AddComponent<SpriteComponent>(
		SpriteComponent{ .sTextureName = "ZZ_S2D_PlayerStart", .width = 64, .height = 64, .layer = 999999 } );

	auto pTexture = MAIN_REGISTRY().GetAssetManager().GetTexture( sprite.sTextureName );
	SCION_ASSERT( pTexture && "ZZ_S2D_PlayerStart texture must be loaded into the asset manager!" );

	GenerateUVs( sprite, pTexture->GetWidth(), pTexture->GetHeight() );

	m_bVisualEntityCreated = true;
}

} // namespace SCION_CORE
