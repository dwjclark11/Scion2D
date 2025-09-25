#pragma once
#include <entt/entt.hpp>
#include <sol/sol.hpp>

namespace SCION_CORE::ECS
{

/*
* Relationship
* Simple struct for an Unconstrained Hierarchy.
* The number of children for the parent is not known and is not clamped.
* Treated like an implicit doubly-linked list with the benefit of
* no dynamic allocations or reallocations.
*/
struct Relationship
{
	/*
	Easy way to get owning entity. This is set when entity is created.
	Not necessary to have, can be removed if desired.
	*/
	entt::entity self{ entt::null };
	/* The first child of the entity. */
	entt::entity firstChild{ entt::null };
	/* The previous sibling entity in the hierarchy for the parent. */
	entt::entity prevSibling{ entt::null };
	/* The next sibling entity in the hierarchy for the parent. */
	entt::entity nextSibling{ entt::null };
	/* The parent entity. Not all entities will have parents. */
	entt::entity parent{ entt::null };

	static void CreateRelationshipLuaBind( sol::state& lua );
};

class Entity;

struct RelationshipUtils
{
	/**
	 * @brief Checks whether one entity is an ancestor of another in the hierarchy.
	 *
	 * Traverses upward from entityA through its parent chain to determine if
	 * entityB is one of its parents.
	 *
	 * @param entityA  The entity whose ancestry will be inspected.
	 * @param entityB  The potential parent entity to check against.
	 * @return true if entityB is a parent (ancestor) of entityA, false otherwise.
	 */
	static bool IsAParentOf( Entity& entityA, Entity& entityB );

	/**
	 * @brief Updates the sibling links to append a child at the end of its sibling list.
	 *
	 * Traverses from the given first child to the last sibling and links the specified
	 * child as the new last sibling in the chain.
	 *
	 * @param firstChild         The first child in the sibling chain used as the traversal starting point.
	 * @param childRelationship  The relationship component of the child being appended.
	 */
	static void SetSiblingLinks( Entity& firstChild, Relationship& childRelationship );

	/**
	* @brief Recursively removes an entity and all of its descendants from the hierarchy,
	*        safely fixing parent/child/sibling links and destroying each entity.
	* 
	* @param entityToRemove The entity to remove and delete along with its entire hierarchy.
	* @param entitiesRemoved Output list that collects the names of every entity removed.
	*/
	static void RemoveAndDelete( Entity& entityToRemove, std::vector<std::string>& entitiesRemoved );

	/**
	* @brief Recursively duplicates an entity and its entire hierarchy into the registry.
	*
	* @param reg The EnTT registry containing the desired entities and components.
	* @param source The source entity to duplicate.
	* @param newParent The parent entity for the top-level copy
	* @return the duplicated entity that serves as the root of the new subtree.
	*/
	static entt::entity DuplicateRecursive( entt::registry& reg, entt::entity source,
											entt::entity newParent = entt::null );

	/*
	* @brief Recursively traverse an entity hierarchy starting from the given root entity and apply
	* a function to that entity.
	*
	* @tparam Func A callable type (lambda, function pointer, or functor).
	* @tparam Args Types of any additional arguments passed to the callable.
	*
	* @param reg Reference to the EnTT registry containing the desired entities.
	* @param root Entity to start traversal from.
	* @param func User-provided callable to execute on each visited entity.
	* @param args Additional arguments to forward to the callable.
	*/
	template <typename Func, typename ...Args>
	static void ApplyFunctionToHierarchy(entt::registry& reg, entt::entity root, Func&& func, Args&&... args)
	{
		// Call user function on the current entity
		std::forward<Func>(func)( root, std::forward<Args>( args )... );

		// Recurse into children
		if (auto* rel = reg.try_get<Relationship>(root))
		{
			// Iterate through the entity hiearchy and apply the callable to each of the children
			for (entt::entity child = rel->firstChild; child != entt::null;
				child = reg.get<Relationship>(child).nextSibling)
			{
				ApplyFunctionToHierarchy( reg, child, std::forward<Func>( func ), std::forward<Args>( args )... );
			}
		}
	}
};

} // namespace SCION_CORE::ECS
