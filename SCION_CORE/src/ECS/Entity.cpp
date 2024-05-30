#include "Core/ECS/Entity.h"
#include "Core/ECS/Components/Identification.h"
#include "Core/ECS/MetaUtilities.h"

using namespace SCION_CORE::Utils;

namespace SCION_CORE::ECS
{

Entity::Entity(Registry& registry)
	: Entity(registry, "GameObject", "")
{
}

Entity::Entity(Registry& registry, const std::string& name, const std::string& group)
	: m_Registry(registry)
	, m_Entity{registry.CreateEntity()}
	, m_sName{name}
	, m_sGroup{group}
{
	AddComponent<Identification>(
		Identification{.name = name, .group = group, .entity_id = static_cast<int32_t>(m_Entity)});
}
Entity::Entity(Registry& registry, const entt::entity& entity)
	: m_Registry(registry)
	, m_Entity(entity)
	, m_sName{""}
	, m_sGroup{""}
{
	if (HasComponent<Identification>())
	{
		const auto& id = GetComponent<Identification>();
		m_sName = id.name;
		m_sGroup = id.group;
	}
}

void Entity::CreateLuaEntityBind(sol::state& lua, Registry& registry)
{
	using namespace entt::literals;
	lua.new_usertype<Entity>(
		"Entity",
		sol::call_constructor,
		sol::factories(
			[ & ](const std::string& name, const std::string& group) {
				return Entity{registry, name, group};
			},
			[ & ](std::int32_t id) {
				return Entity{registry, static_cast<entt::entity>(id)};
			}),
		"add_component",
		[](Entity& entity, const sol::table& comp, sol::this_state s) -> sol::object {
			if (!comp.valid())
				return sol::lua_nil_t{};

			const auto component = InvokeMetaFunction(GetIdType(comp), "add_component"_hs, entity, comp, s);

			return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"has_component",
		[](Entity& entity, const sol::table& comp) {
			const auto has_comp = InvokeMetaFunction(GetIdType(comp), "has_component"_hs, entity);

			return has_comp ? has_comp.cast<bool>() : false;
		},
		"get_component",
		[](Entity& entity, const sol::table& comp, sol::this_state s) {
			const auto component = InvokeMetaFunction(GetIdType(comp), "get_component"_hs, entity, s);

			return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"remove_component",
		[](Entity& entity, const sol::table& comp) {
			const auto component = InvokeMetaFunction(GetIdType(comp), "remove_component"_hs, entity);

			return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"name",
		&Entity::GetName,
		"group",
		&Entity::GetGroup,
		"kill",
		&Entity::Kill,
		"id",
		[](Entity& entity) { return static_cast<int32_t>(entity.GetEntity()); });
}
} // namespace SCION_CORE::ECS