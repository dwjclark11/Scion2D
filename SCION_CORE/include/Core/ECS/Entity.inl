#pragma once
#include "Entity.h"

namespace SCION_CORE::ECS {
	
	template <typename TComponent, typename ...Args>
	TComponent& Entity::AddComponent(Args&& ...args)
	{
		auto& registry = m_Registry.GetRegistry();
		return registry.emplace<TComponent>(m_Entity, std::forward<Args>(args) ...);
	}

	template <typename TComponent, typename ...Args>
	TComponent& Entity::ReplaceComponent(Args&& ...args)
	{
		auto& registry = m_Registry.GetRegistry();
		if (registry.all_of<TComponent>(m_Entity))
			return registry.replace<TComponent>(m_Entity, std::forward<Args>(args) ...);
		else
			return registry.emplace<TComponent>(m_Entity, std::forward<Args>(args) ...);
	}

	template <typename TComponent>
	TComponent& Entity::GetComponent()
	{
		auto& registry = m_Registry.GetRegistry();
		return registry.get<TComponent>(m_Entity);
	}

	template <typename TComponent>
	bool Entity::HasComponent()
	{
		auto& registry = m_Registry.GetRegistry();
		return registry.all_of<TComponent>(m_Entity);
	}

	template <typename TComponent>
	auto Entity::RemoveComponent()
	{
		auto& registry = m_Registry.GetRegistry();
		return registry.remove<TComponent>(m_Entity);
	}
	
	template<typename TComponent>
	auto add_component(Entity& entity, const sol::table& comp, sol::this_state s)
	{
		auto& component = entity.AddComponent<TComponent>(
			comp.valid() ? comp.as<TComponent>() : TComponent{}
		);

		return sol::make_reference(s, std::ref(component));
	}

	template<typename TComponent>
	bool has_component(Entity& entity)
	{
		return entity.HasComponent<TComponent>();
	}

	template<typename TComponent>
	auto get_component(Entity& entity, sol::this_state s)
	{
		auto& comp = entity.GetComponent<TComponent>();
		return sol::make_reference(s, std::ref(comp));
	}

	template<typename TComponent>
	auto remove_component(Entity& entity)
	{
		return entity.RemoveComponent<TComponent>();
	}

	template<typename TComponent>
	inline void Entity::RegisterMetaComponent()
	{
		using namespace entt::literals;
		entt::meta<TComponent>()
			.type(entt::type_hash<TComponent>::value())
			.template func<&add_component<TComponent>>("add_component"_hs)
			.template func<&has_component<TComponent>>("has_component"_hs)
			.template func<&get_component<TComponent>>("get_component"_hs)
			.template func<&remove_component<TComponent>>("remove_component"_hs);
	}
}