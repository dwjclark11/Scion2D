#include "GameObjectCmds.h"
#include "editor/scene/SceneObject.h"
#include "Core/ECS/Registry.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/CoreUtilities/CoreUtilities.h"

#include "Logger/Logger.h"

using namespace SCION_CORE;
using namespace SCION_CORE::ECS;

namespace SCION_EDITOR
{

void AddGameObjectCmd::undo()
{
	SCION_ASSERT( pSceneObject && "The SceneObject cannot be nullptr." );

	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to undo remove tile layer. SceneObject was not set correctly." );
		return;
	}

	SCION_ASSERT( pGameObjectData->id && "GameObject Identification component must be set." );
	pSceneObject->DeleteGameObjectByTag( pGameObjectData->id->name );
}

void AddGameObjectCmd::redo()
{
	SCION_ASSERT( pSceneObject && "The SceneObject cannot be nullptr." );

	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to undo remove tile layer. SceneObject was not set correctly." );
		return;
	}

	SCION_ASSERT( pGameObjectData->id && "GameObject Identification component must be set." );
	auto* pRegistry = pSceneObject->GetRegistryPtr();

	Entity entity{ *pRegistry, pGameObjectData->id->name, pGameObjectData->id->group };
	entity.ChangeGUID( pGameObjectData->id->sGUID );
	entity.AddComponent<TransformComponent>( *pGameObjectData->transform );

	pSceneObject->AddGameObjectByTag( pGameObjectData->id->name, entity.GetEntity() );
}

void DeleteGameObjectCmd::undo()
{
	SCION_ASSERT( pSceneObject && "The SceneObject cannot be nullptr." );

	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to undo remove tile layer. SceneObject was not set correctly." );
		return;
	}

	auto* pRegistry = pSceneObject->GetRegistryPtr();
	std::vector<Entity> newEntities;
	newEntities.reserve( mapTagToGameObjectData.size() );

	for ( const auto& [ sTag, pGameObjectData ] : mapTagToGameObjectData )
	{
		SCION_ASSERT( pGameObjectData && pGameObjectData->id && "GameObject Identification component must be set." );
		Entity entity{ *pRegistry, pGameObjectData->id->name, pGameObjectData->id->group };
		entity.ChangeGUID( pGameObjectData->id->sGUID );
		entity.AddComponent<TransformComponent>( *pGameObjectData->transform );

		if ( pGameObjectData->sprite )
		{
			entity.AddComponent<SpriteComponent>( *pGameObjectData->sprite );
		}

		if ( pGameObjectData->boxCollider )
		{
			entity.AddComponent<BoxColliderComponent>( *pGameObjectData->boxCollider );
		}

		if ( pGameObjectData->circleCollider )
		{
			entity.AddComponent<CircleColliderComponent>( *pGameObjectData->circleCollider );
		}

		if ( pGameObjectData->animation )
		{
			entity.AddComponent<AnimationComponent>( *pGameObjectData->animation );
		}

		if ( pGameObjectData->physics )
		{
			entity.AddComponent<PhysicsComponent>( *pGameObjectData->physics );
		}

		if ( pGameObjectData->ui )
		{
			entity.AddComponent<UIComponent>( *pGameObjectData->ui );
		}

		if ( pGameObjectData->text )
		{
			entity.AddComponent<TextComponent>( *pGameObjectData->text );
		}

		if ( !pSceneObject->AddGameObjectByTag( pGameObjectData->id->name, entity.GetEntity() ) )
		{
			SCION_ERROR( "Failed to add game object [{}]", pGameObjectData->id->name );
			continue;
		}

		newEntities.emplace_back( entity );
	}

	// Handle links
	for ( auto& entity : newEntities )
	{
		auto entItr = mapTagToGameObjectData.find( entity.GetName() );
		if ( entItr == mapTagToGameObjectData.end() || !entItr->second->relationship )
			continue;

		auto& relationships = entity.GetComponent<Relationship>();
		const auto& gameObjectRelations = *entItr->second->relationship;
		if ( !gameObjectRelations.sParent.empty() )
		{
			auto itr = std::ranges::find_if(
				newEntities, [ & ]( const auto& ent ) { return ent.GetName() == gameObjectRelations.sParent; } );

			if ( itr != newEntities.end() )
			{
				relationships.parent = itr->GetEntity();
			}
			else // We need to seach the scene for the parent
			{
				auto parEntity = pSceneObject->GetGameObjectEntityByTag( gameObjectRelations.sParent );
				SCION_ASSERT( parEntity != entt::null && "The parent must exist if set before deletion." );
				Entity parentEnt{ *pRegistry, parEntity };
				parentEnt.AddChild( relationships.self );
			}
		}

		if ( !gameObjectRelations.sFirstChild.empty() )
		{
			auto itr = std::ranges::find_if(
				newEntities, [ & ]( const auto& ent ) { return ent.GetName() == gameObjectRelations.sFirstChild; } );

			if ( itr != newEntities.end() )
			{
				relationships.firstChild = itr->GetEntity();
			}
		}

		if ( !gameObjectRelations.sPrevSibling.empty() )
		{
			auto itr = std::ranges::find_if(
				newEntities, [ & ]( const auto& ent ) { return ent.GetName() == gameObjectRelations.sPrevSibling; } );

			if ( itr != newEntities.end() )
			{
				relationships.prevSibling = itr->GetEntity();
			}
		}

		if ( !gameObjectRelations.sNextSibling.empty() )
		{
			auto itr = std::ranges::find_if(
				newEntities, [ & ]( const auto& ent ) { return ent.GetName() == gameObjectRelations.sNextSibling; } );

			if ( itr != newEntities.end() )
			{
				relationships.nextSibling = itr->GetEntity();
			}
		}
	}
}

void DeleteGameObjectCmd::redo()
{
	SCION_ASSERT( pSceneObject && "The SceneObject cannot be nullptr." );

	if ( !pSceneObject )
	{
		SCION_ERROR( "Failed to undo remove tile layer. SceneObject was not set correctly." );
		return;
	}

	auto* pRegistry = pSceneObject->GetRegistryPtr();
}

} // namespace SCION_EDITOR
