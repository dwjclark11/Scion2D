#include "Core/ECS/Entity.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MetaUtilities.h"

#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/Scene/Scene.h"

using namespace Scion::Core::Utils;

namespace Scion::Core::ECS
{

Entity::Entity( Registry* registry )
	: Entity( registry, "GameObject", "" )
{
}

Entity::Entity( Registry* registry, const std::string& name, const std::string& group )
	: m_Registry{ registry }
	, m_Entity{ registry->CreateEntity() }
	, m_sName{ name }
	, m_sGroup{ group }
{
	AddComponent<Identification>(
		Identification{ .name = name, .group = group, .entity_id = static_cast<uint32_t>( m_Entity ) } );

	/* Add Relationship component and set self to underlying entt::entity. */
	AddComponent<Relationship>( Relationship{ .self = m_Entity } );
}

Entity::Entity( Registry* registry, const entt::entity& entity )
	: m_Registry{ registry }
	, m_Entity{ entity }
	, m_sName{}
	, m_sGroup{}
{
	if ( HasComponent<Identification>() )
	{
		const auto& id = GetComponent<Identification>();
		m_sName = id.name;
		m_sGroup = id.group;
	}
}

Entity::Entity( const Entity& other )
	: m_Registry{ other.m_Registry }
	, m_Entity{ other.m_Entity }
	, m_sName{ other.m_sName }
	, m_sGroup{ other.m_sGroup }
{

}

Entity& Entity::operator=( const Entity& other )
{
	if (this != &other)
	{
		this->m_Registry = other.m_Registry;
		this->m_Entity = other.m_Entity;
		this->m_sName = other.m_sName;
		this->m_sGroup = other.m_sGroup;
	}

	return *this;
}

Entity::Entity( Entity&& other ) noexcept
	: m_Registry{ other.m_Registry }
	, m_Entity{ other.m_Entity }
	, m_sName{ std::move(other.m_sName) }
	, m_sGroup{ std::move(other.m_sGroup)}
{
	other.m_Registry = nullptr;
	other.m_Entity = entt::null;
	other.m_sName.clear();
	other.m_sGroup.clear();
}

Entity& Entity::operator=( Entity&& other ) noexcept
{
	if (this != &other)
	{

	}

	return *this;
}

Entity::~Entity() {

}

bool Entity::AddChild( entt::entity child, bool bSetLocal )
{
	auto& registry = m_Registry->GetRegistry();
	if (auto* pChildUneditable = registry.try_get<UneditableComponent>(child))
	{
		SCION_WARN( "Failed to add child. Child is an uneditable entity." );
		return false;
	}

	if ( auto* pParentUneditable = registry.try_get<UneditableComponent>( m_Entity ) )
	{
		SCION_WARN( "Failed to add child. Parent is an uneditable entity." );
		return false;
	}

	auto& relations = registry.get<Relationship>( m_Entity );

	Entity childEntity{ m_Registry, child };
	auto& childRelationship = childEntity.GetComponent<Relationship>();

	if ( RelationshipUtils::IsAParentOf( *this, childEntity ) )
	{
		SCION_ERROR( "Failed to add child. Cannot make a parent into a child." );
		return false;
	}

	// Check to see if the parent is already this entity
	if ( childRelationship.parent == m_Entity )
	{
		// If the child is the first child, grab the next child and adjust links
		if ( relations.firstChild == child )
		{
			relations.firstChild = childRelationship.nextSibling;
		}
		else
		{
			// Handle the child and its siblings.
			if ( childRelationship.prevSibling != entt::null )
			{
				auto& prev = registry.get<Relationship>( childRelationship.prevSibling );
				prev.nextSibling = childRelationship.nextSibling;
			}

			if ( childRelationship.nextSibling != entt::null )
			{
				auto& next = registry.get<Relationship>( childRelationship.nextSibling );
				next.prevSibling = childRelationship.prevSibling;
			}
		}

		// Reset the child's siblings
		childRelationship.prevSibling = entt::null;
		childRelationship.nextSibling = entt::null;

		childRelationship.parent = relations.parent;
		if ( auto* parent = registry.try_get<Relationship>( relations.parent ) )
		{
			if ( parent->firstChild != entt::null )
			{
				Entity firstChild{ m_Registry, parent->firstChild };
				RelationshipUtils::SetSiblingLinks( firstChild, childRelationship );
			}
		}

		// Set the childs local position
		auto& childTransform = childEntity.GetComponent<TransformComponent>();
		if ( relations.parent != entt::null && bSetLocal )
		{
			childTransform.localPosition =
				childTransform.position - registry.get<TransformComponent>( relations.parent ).position;
			childTransform.localRotation = childTransform.rotation;
		}

		return true;
	}

	// Handle the child and its siblings.
	if ( childRelationship.prevSibling != entt::null )
	{
		if ( auto* prev = registry.try_get<Relationship>( childRelationship.prevSibling ) )
		{
			prev->nextSibling = childRelationship.nextSibling;
		}
	}

	if ( childRelationship.nextSibling != entt::null )
	{
		if ( auto* next = registry.try_get<Relationship>( childRelationship.nextSibling ) )
		{
			next->prevSibling = childRelationship.prevSibling;
		}
	}

	// Remove the child count from the parent
	if ( childRelationship.parent != entt::null )
	{
		auto& parent = registry.get<Relationship>( childRelationship.parent );
		if ( parent.firstChild == child )
		{
			parent.firstChild = entt::null;
			// Get the next sibling and set the links
			if ( auto* nextSibling = registry.try_get<Relationship>( childRelationship.nextSibling ) )
			{
				parent.firstChild = childRelationship.nextSibling;
				nextSibling->prevSibling = entt::null;
			}
		}
	}

	// Reset the child's siblings, they will change later
	childRelationship.nextSibling = entt::null;
	childRelationship.prevSibling = entt::null;

	// Set the parent to the new entity
	childRelationship.parent = m_Entity;

	// Set the childs local position
	auto& childTransform = childEntity.GetComponent<TransformComponent>();
	if ( bSetLocal )
	{
		childTransform.localPosition = childTransform.position - GetComponent<TransformComponent>().position;
		childTransform.localRotation = childTransform.rotation;
	}

	// Check to see if the parent has any children
	// Parent has no children, add as the first child
	if ( relations.firstChild == entt::null )
	{
		relations.firstChild = child;
	}
	else // If the parent already has a child, we need to find the last location and set
	{
		// we want to get a copy of the parentChild here so we can move to the next one
		Entity firstChild{ m_Registry, relations.firstChild };
		RelationshipUtils::SetSiblingLinks( firstChild, childRelationship );
	}

	return true;
}

void Entity::UpdateTransform()
{
	auto& relations = GetComponent<Relationship>();
	auto& transform = GetComponent<TransformComponent>();

	//glm::vec2 parentPosition{ 0.f };
	auto parent = relations.parent;
	if ( parent != entt::null )
	{
		Entity ent{ m_Registry, parent };
		const auto& parentTransform = ent.GetComponent<TransformComponent>();
		transform.position = parentTransform.position + transform.localPosition;
		transform.rotation = parentTransform.rotation + transform.localRotation; 
	}

	if ( relations.firstChild == entt::null )
		return;

	auto child = relations.firstChild;
	while ( child != entt::null )
	{
		Entity childEnt{ m_Registry, child };
		childEnt.UpdateTransform();
		child = childEnt.GetComponent<Relationship>().nextSibling;
	}
}

void Entity::ChangeName( const std::string& sName )
{
	auto& id = GetComponent<Identification>();
	id.name = sName;
	m_sName = sName;
}

void Entity::Destroy()
{
	if ( !m_Registry->IsValid( m_Entity ) )
	{
		SCION_ERROR( "Failed t destroy entity. Entity ID [{}] is not valid.", static_cast<std::uint32_t>( m_Entity ) );
		return;
	}

	// TODO: Handle Deleting children etc
	m_Registry->AddToPendingDestruction( m_Entity );
}

void Entity::CreateLuaEntityBind( sol::state& lua, Registry& registry )
{
	using namespace entt::literals;
	lua.new_usertype<Entity>(
		"Entity",
		sol::call_constructor,
		sol::factories(
			[ & ]( Registry& reg, const std::string& sName, const std::string& sGroup ) {
				return Entity{ &reg, sName, sGroup };
			},
			[ & ]( const std::string& name, const std::string& group ) { return Entity{ &registry, name, group }; },
			[ & ]( std::uint32_t id ) { return Entity{ &registry, static_cast<entt::entity>( id ) }; } ),
		"addComponent",
		[]( Entity& entity, const sol::table& comp, sol::this_state s ) -> sol::object {
			if ( !comp.valid() )
				return sol::lua_nil_t{};

			const auto component = InvokeMetaFunction( GetIdType( comp ), "add_component"_hs, entity, comp, s );

			return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"hasComponent",
		[]( Entity& entity, const sol::table& comp ) {
			const auto has_comp = InvokeMetaFunction( GetIdType( comp ), "has_component"_hs, entity );

			return has_comp ? has_comp.cast<bool>() : false;
		},
		"getComponent",
		[]( Entity& entity, const sol::table& comp, sol::this_state s ) {
			const auto component = InvokeMetaFunction( GetIdType( comp ), "get_component"_hs, entity, s );

			return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"removeComponent",
		[]( Entity& entity, const sol::table& comp ) {
			const auto component = InvokeMetaFunction( GetIdType( comp ), "remove_component"_hs, entity );

			return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"name",
		&Entity::GetName,
		"group",
		&Entity::GetGroup,
		"destroy",
		&Entity::Destroy,
		"addChild",
		[]( Entity& entity, Entity& child ) { entity.AddChild( child.GetEntity() ); },
		"updateTransform",
		&Entity::UpdateTransform,
		"updateIsoSorting",
		[]( Entity& entity, const Canvas& canvas ) {
			if ( auto* pSprite = entity.TryGetComponent<SpriteComponent>(); pSprite->bIsoMetric )
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				auto [ cellX, cellY ] = Scion::Core::ConvertWorldPosToIsoCoords( transform.position, canvas );
				pSprite->isoCellX = cellX;
				pSprite->isoCellY = cellY;
			}
			else
			{
				SCION_ERROR( "Entity does not have a sprite component or is not using iso sorting." );
			}
		},
		"id",
		[]( Entity& entity ) { return static_cast<uint32_t>( entity.GetEntity() ); } );
}
} // namespace Scion::Core::ECS
