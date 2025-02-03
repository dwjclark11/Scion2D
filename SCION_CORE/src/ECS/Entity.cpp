#include "Core/ECS/Entity.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MetaUtilities.h"

using namespace SCION_CORE::Utils;

namespace SCION_CORE::ECS
{

Entity::Entity( Registry& registry )
	: Entity( registry, "GameObject", "" )
{
}

Entity::Entity( Registry& registry, const std::string& name, const std::string& group )
	: m_Registry( registry )
	, m_Entity{ registry.CreateEntity() }
	, m_sName{ name }
	, m_sGroup{ group }
{
	AddComponent<Identification>(
		Identification{ .name = name, .group = group, .entity_id = static_cast<uint32_t>( m_Entity ) } );

	/* Add Relationship component and set self to underlying entt::entity. */
	AddComponent<Relationship>( Relationship{ .self = m_Entity } );
}

Entity::Entity( Registry& registry, const entt::entity& entity )
	: m_Registry( registry )
	, m_Entity( entity )
	, m_sName{ "" }
	, m_sGroup{ "" }
{
	if ( HasComponent<Identification>() )
	{
		const auto& id = GetComponent<Identification>();
		m_sName = id.name;
		m_sGroup = id.group;
	}
}

bool Entity::AddChild( entt::entity child )
{
	auto& registry = m_Registry.GetRegistry();
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
		if ( relations.parent != entt::null )
		{
			childTransform.localPosition =
				childTransform.position - registry.get<TransformComponent>( relations.parent ).position;
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
	childTransform.localPosition = childTransform.position - GetComponent<TransformComponent>().position;

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

	glm::vec2 parentPosition{ 0.f };
	auto parent = relations.parent;
	if ( parent != entt::null )
	{
		Entity ent{ m_Registry, parent };
		parentPosition = ent.GetComponent<TransformComponent>().position;
		transform.position = parentPosition + transform.localPosition;
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

void Entity::CreateLuaEntityBind( sol::state& lua, Registry& registry )
{
	using namespace entt::literals;
	lua.new_usertype<Entity>(
		"Entity",
		sol::call_constructor,
		sol::factories(
			[ & ]( Registry& reg, const std::string& sName, const std::string& sGroup ) {
				return Entity{ reg, sName, sGroup };
			},
			[ & ]( const std::string& name, const std::string& group ) { return Entity{ registry, name, group }; },
			[ & ]( std::uint32_t id ) { return Entity{ registry, static_cast<entt::entity>( id ) }; } ),
		"add_component",
		[]( Entity& entity, const sol::table& comp, sol::this_state s ) -> sol::object {
			if ( !comp.valid() )
				return sol::lua_nil_t{};

			const auto component = InvokeMetaFunction( GetIdType( comp ), "add_component"_hs, entity, comp, s );

			return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"has_component",
		[]( Entity& entity, const sol::table& comp ) {
			const auto has_comp = InvokeMetaFunction( GetIdType( comp ), "has_component"_hs, entity );

			return has_comp ? has_comp.cast<bool>() : false;
		},
		"get_component",
		[]( Entity& entity, const sol::table& comp, sol::this_state s ) {
			const auto component = InvokeMetaFunction( GetIdType( comp ), "get_component"_hs, entity, s );

			return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"remove_component",
		[]( Entity& entity, const sol::table& comp ) {
			const auto component = InvokeMetaFunction( GetIdType( comp ), "remove_component"_hs, entity );

			return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"name",
		&Entity::GetName,
		"group",
		&Entity::GetGroup,
		"kill",
		&Entity::Kill,
		"addChild",
		[]( Entity& entity, Entity& child ) { entity.AddChild( child.GetEntity() ); },
		"updateTransform",
		&Entity::UpdateTransform,
		"id",
		[]( Entity& entity ) { return static_cast<uint32_t>( entity.GetEntity() ); } );
}
} // namespace SCION_CORE::ECS
