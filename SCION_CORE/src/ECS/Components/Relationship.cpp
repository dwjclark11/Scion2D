#include "Core/ECS/Components/Relationship.h"
#include "Core/ECS/Entity.h"

namespace SCION_CORE::ECS
{
bool RelationshipUtils::IsAParentOf( Entity& entityA, Entity& entityB )
{
	auto& registry = entityA.GetEnttRegistry();
	auto entity = entityB.GetEntity();
	const auto& relations = entityA.GetComponent<Relationship>();

	// Get a copy to the parent entity
	auto parentEnt = relations.parent;

	while ( parentEnt != entt::null )
	{
		if ( parentEnt == entity )
		{
			return true;
		}

		const auto& parentRelations = registry.get<Relationship>( parentEnt );
		parentEnt = parentRelations.parent;
	}

	return false;
}
void RelationshipUtils::SetSiblingLinks( Entity& firstChild, Relationship& childRelationship )
{
	// we want to get a copy of the parentChild here so we can move to the next one
	auto& registry = firstChild.GetEnttRegistry();

	auto parentChild = registry.get<Relationship>( firstChild.GetEntity() );

	while ( parentChild.nextSibling != entt::null )
	{
		parentChild = registry.get<Relationship>( parentChild.nextSibling );
	}

	// Since this is a copy, we need to get a reference to make the changes
	auto& prevSibling = registry.get<Relationship>( parentChild.self );
	prevSibling.nextSibling = childRelationship.self;
	childRelationship.prevSibling = prevSibling.self;
}

void RelationshipUtils::RemoveAndDelete( Entity& entityToRemove, std::vector<std::string>& entitiesRemoved )
{
	auto& registry = entityToRemove.GetEnttRegistry();
	auto& Reg = entityToRemove.GetRegistry();
	auto& relations = entityToRemove.GetComponent<Relationship>();

	// First we need to delete all of the children
	if ( relations.firstChild != entt::null )
	{
		std::vector<entt::entity> childrenToDelete{};
		childrenToDelete.push_back( relations.firstChild );
		auto childRelations = registry.get<Relationship>( relations.firstChild );
		while ( childRelations.nextSibling != entt::null )
		{
			childrenToDelete.push_back( childRelations.nextSibling );
			childRelations = registry.get<Relationship>( childRelations.nextSibling );
		}

		for ( auto entity : childrenToDelete )
		{
			Entity ent{ &Reg, entity };
			RemoveAndDelete( ent, entitiesRemoved );
		}
	}

	// Now we need to fix any links to the entity
	if ( relations.parent != entt::null )
	{
		auto& parent = registry.get<Relationship>( relations.parent );

		// If the child is the first child, grab the next child and adjust links
		if ( parent.firstChild == relations.self )
		{
			parent.firstChild = relations.nextSibling;
		}
		else
		{
			// Handle the child and its siblings.
			if ( relations.prevSibling != entt::null )
			{
				auto& prev = registry.get<Relationship>( relations.prevSibling );
				prev.nextSibling = relations.nextSibling;
			}

			if ( relations.nextSibling != entt::null )
			{
				auto& next = registry.get<Relationship>( relations.nextSibling );
				next.prevSibling = relations.prevSibling;
			}
		}

		relations.prevSibling = entt::null;
		relations.nextSibling = entt::null;
	}

	// Now that the links have been adjusted, we can delete the entity
	//SCION_LOG( "JUST Destroyed: {}", static_cast<std::uint32_t>( entityToRemove.GetEntity() ) );
	entitiesRemoved.emplace_back( entityToRemove.GetName() );
	entityToRemove.Destroy();
}

void Relationship::CreateRelationshipLuaBind( sol::state& lua )
{
	lua.new_usertype<Relationship>(
		"Relationship",
		"type_id",
		&entt::type_hash<Relationship>::value,
		sol::call_constructor,
		sol::constructors<Relationship()>(),
		"self",
		[]( Relationship& relations ) { return static_cast<uint32_t>( relations.self ); },
		"parent",
		[]( Relationship& relations ) { return static_cast<uint32_t>( relations.parent ); },
		"nextSibling",
		[]( Relationship& relations ) { return static_cast<uint32_t>( relations.nextSibling ); },
		"prevSibling",
		[]( Relationship& relations ) { return static_cast<uint32_t>( relations.prevSibling ); },
		"firstChild",
		[]( Relationship& relations ) { return static_cast<uint32_t>( relations.firstChild ); } );
}

} // namespace SCION_CORE::ECS
