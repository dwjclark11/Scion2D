#pragma once
#include <entt/entt.hpp>
#include <sol/sol.hpp>

namespace SCION_CORE::ECS
{

enum ERegistryType
{
	LuaRegistry,
	ScionRegistry
};

class Registry
{
  public:
	Registry();
	~Registry() = default;

	/*
	 * @brief Checks to see if the entity is valid.
	 * @param the entity to check.
	 * @return Returns true if the entity is valid, false otherwise.
	 */
	inline bool IsValid( entt::entity entity ) const { return m_pRegistry->valid( entity ); }

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

	void ClearRegistry();
	void AddToPendingDestruction( entt::entity entity );
	void ClearPendingEntities();

	/*
	 * @brief The context is a general purpose map that can hold any type of variable.
	 * The variable must be movable and constructable.
	 * @tparam The template parameter is the type of variable that you want to hold in the map.
	 * This can be any type. EX: std::shared_ptr<YourType>.
	 * @return Returns a reference to the newly added context.
	 */
	template <typename TContext>
	TContext AddToContext( TContext context );

	/*
	 * @brief Checks the map for the desired context variable and returns
	 * that if it exists in the map.
	 * @return Returns the desired context as a reference.
	 * @throw This will throw an exception if the given context does not exist
	 * or is not available.
	 */
	template <typename TContext>
	TContext& GetContext();

	template <typename TContext>
	TContext* TryGetContext();

	template <typename TContext>
	bool RemoveContext();

	template <typename TContext>
	bool HasContext();

	template <typename... Excludes>
	void DestroyEntities();

	static void CreateLuaRegistryBind( sol::state& lua, Registry& registry );

	template <typename TComponent>
	static void RegisterMetaComponent();

  private:
	std::shared_ptr<entt::registry> m_pRegistry;
	ERegistryType m_eType{ ERegistryType::ScionRegistry };
	std::vector<entt::entity> m_EntitiesPendingDestruction;
};

template <typename TComponent>
entt::runtime_view& add_component_to_view( Registry* registry, entt::runtime_view& view );

template <typename TComponent>
auto exclude_component_from_view( Registry* registry, entt::runtime_view* view );

} // namespace SCION_CORE::ECS

#include "Registry.inl"
