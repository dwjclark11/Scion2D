#pragma once
#include "Registry.h"
#include <sol/sol.hpp>

namespace SCION_CORE::ECS
{
class Entity
{
  private:
	Registry& m_Registry;
	entt::entity m_Entity;
	std::string m_sName, m_sGroup;

  public:
	Entity( Registry& registry );
	Entity( Registry& registry, const std::string& name = "", const std::string& group = "" );

	Entity( Registry& registry, const entt::entity& entity );
	Entity& operator=( const Entity& other )
	{
		this->m_Entity = other.m_Entity;
		this->m_sName = other.m_sName;
		this->m_sGroup = other.m_sGroup;

		return *this;
	}

	~Entity() = default;

	inline const std::string& GetName() const { return m_sName; }
	inline const std::string& GetGroup() const { return m_sGroup; }
	/*
	 * @brief Destroys the underlying entt::entity. This will remove the entity from the
	 * the registry. USE WITH CAUTION!! Please ensure that there are no other references to
	 * this Entity, trying to access an entity that does not exist can cause problems.
	 * @return Returns the std::uint32_t id of the destroyed entity.
	 */
	inline std::uint32_t Kill() { return m_Registry.GetRegistry().destroy( m_Entity ); }

	/*
	 * @brief Gets the actual entity.
	 * @return Returns the underlying entt::entity.
	 */
	inline entt::entity& GetEntity() { return m_Entity; }

	/*
	 * @brief All entities have a reference to the registry that they were created in.
	 * @return Returns the actual underlying entt::registry as a reference.
	 */
	inline entt::registry& GetRegistry() { return m_Registry.GetRegistry(); }

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

	template <typename TComponent>
	TComponent& GetComponent();

	/*
	 * @brief Checks to see if the entity has the given component.
	 * @tparam The desired component to check.
	 * @return Returns true if the entity has that component, false otherwise
	 */
	template <typename TComponent>
	bool HasComponent();

	template <typename TComponent>
	auto RemoveComponent();
};

template <typename TComponent>
auto add_component( Entity& entity, const sol::table& comp, sol::this_state s );

template <typename TComponent>
auto add_component_default( Entity& entity);

template <typename TComponent>
bool has_component( Entity& entity );

template <typename TComponent>
auto get_component( Entity& entity, sol::this_state s );

template <typename TComponent>
auto remove_component( Entity& entity );

template <typename TComponent>
auto copy_component( Entity& entityToCopy, Entity& entityThatCopies );

} // namespace SCION_CORE::ECS

#include "Entity.inl"
