#pragma once
#include <entt/entt.hpp>
#include <sol/sol.hpp>

namespace SCION_CORE::ECS
{
class Registry
{
  private:
	std::unique_ptr<entt::registry> m_pRegistry;

  public:
	Registry();
	~Registry() = default;

	/*
	 * @brief Get the actual registry
	 * @return Returns the underlying entt::registry
	 */
	inline entt::registry& GetRegistry() { return *m_pRegistry; }

	/*
	 * @brief Creates a new entity and adds it to the registry.
	 * @return Returns the newly created entt::entity.
	 */
	inline entt::entity CreateEntity() { return m_pRegistry->create(); }

	/*
	 * @brief The context is a general purpose map that can hold any type of variable.
	 * The variable must be movable and constructable.
	 * @tparam The template parameter is the type of variable that you want to hold in the map.
	 * This can be any type. EX: std::shared_ptr<YourType>.
	 * @return Returns a reference to the newly added context.
	 */
	template <typename TContext>
	TContext AddToContext(TContext context);

	/*
	 * @brief Checks the map for the desired context variable and returns
	 * that if it exists in the map.
	 * @return Returns the desired context as a reference.
	 * @throw This will throw an exception if the given context does not exist
	 * or is not available.
	 */
	template <typename TContext>
	TContext& GetContext();

	static void CreateLuaRegistryBind(sol::state& lua, Registry& registry);

	template <typename TComponent>
	static void RegisterMetaComponent();
};

template <typename TComponent>
entt::runtime_view& add_component_to_view(Registry* registry, entt::runtime_view& view);

template <typename TComponent>
entt::runtime_view& exclude_component_from_view(Registry* registry, entt::runtime_view& view);

} // namespace SCION_CORE::ECS

#include "Registry.inl"