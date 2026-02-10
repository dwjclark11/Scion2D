#pragma once
#include "Registry.h"
#include <sol/sol.hpp>

namespace Scion::Core::ECS
{
class Entity
{
  public:
	Entity( Registry* registry );
	Entity( Registry* registry, const std::string& name = "", const std::string& group = "" );
	Entity( Registry* registry, const entt::entity& entity );

	Entity( const Entity& other );
	Entity& operator=( const Entity& other );

	Entity( Entity&& other ) noexcept;
	Entity& operator=( Entity&& other ) noexcept;

	virtual ~Entity();
	
	/*
	 * @brief Adds a new child to the entity.
	 * @param underlying entity of the child to add.
	 */
	bool AddChild( entt::entity child, bool bSetLocal = true );

	/*
	 * @brief Updates the position of the entity. If the entity
	 * has children, it will update all the children as well.
	 */
	void UpdateTransform();

	void ChangeName( const std::string& sName );

	inline const std::string& GetName() const { return m_sName; }
	inline const std::string& GetGroup() const { return m_sGroup; }
	/*
	 * @brief Destroys the underlying entt::entity. This will remove the entity from the
	 * the registry. USE WITH CAUTION!! Please ensure that there are no other references to
	 * this Entity, trying to access an entity that does not exist can cause problems.
	 */
	void Destroy();
	
	/*
	 * @brief Gets the actual entity.
	 * @return Returns the underlying entt::entity.
	 */
	inline entt::entity& GetEntity() { return m_Entity; }

	/*
	 * @brief All entities have a reference to the registry that they were created in.
	 * @return Returns the actual underlying entt::registry as a reference.
	 */
	inline entt::registry& GetEnttRegistry() { return m_Registry->GetRegistry(); }

	inline Registry& GetRegistry() { return *m_Registry; }

	static void CreateLuaEntityBind( sol::state& lua, Registry& registry );

	template <typename TComponent>
	static void RegisterMetaComponent();

	/*
	 * @brief Add a component to entity.
	 * @tparam Takes a TComponent parameter and the arguments neeeded to
	 * construct the given component.
	 * @return Returns a reference to the added component.
	 */
	template <typename TComponent, typename... Args>
	TComponent& AddComponent( Args&&... args );

	template <typename TComponent, typename... Args>
	TComponent& ReplaceComponent( Args&&... args );

	/*
	 * @brief Attempts to get the desired component.
	 * @tparam Component Type to retrieve.
	 * @return Returns a reference to the desired component. If the entity does not have the
	 * component, this will result in undefined behavior.
	 */
	template <typename TComponent>
	TComponent& GetComponent();

	/*
	 * @brief Attempts to get the desired component.
	 * @tparam Component Type to retrieve.
	 * @return Returns a pointer to the desired component if the entity has one, or else returns nullptr.
	 */
	template <typename TComponent>
	TComponent* TryGetComponent();

	/*
	 * @brief Checks to see if the entity has the given component.
	 * @tparam The desired component to check.
	 * @return Returns true if the entity has that component, false otherwise
	 */
	template <typename TComponent>
	bool HasComponent();

	template <typename TComponent>
	auto RemoveComponent();

  private:
	/* Pointer to the registry this entity belongs to. */
	Registry* m_Registry;
	/* Underlying entity. */
	entt::entity m_Entity;
	/* Entities specific name. Eventually they will be unique names. */
	std::string m_sName;
	/* The group this entity belongs to. We could add multiple groups if needed later. */
	std::string m_sGroup;
};

template <typename TComponent>
auto add_component( Entity& entity, const sol::table& comp, sol::this_state s );

template <typename TComponent>
auto add_component_default( Entity& entity );

template <typename TComponent>
bool has_component( Entity& entity );

template <typename TComponent>
auto get_component( Entity& entity, sol::this_state s );

template <typename TComponent>
auto remove_component( Entity& entity );

template <typename TComponent>
auto copy_component( Entity& entityToCopy, Entity& entityThatCopies );

} // namespace Scion::Core::ECS

#include "Entity.inl"
