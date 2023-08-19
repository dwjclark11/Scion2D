#include "Entity.h"
#include "Components/Identification.h"

namespace SCION_CORE::ECS {

	Entity::Entity(Registry& registry)
		: Entity(registry, "GameObject", "")
	{
	}

	Entity::Entity(Registry& registry, const std::string& name, const std::string& group)
		: m_Registry(registry), m_Entity{registry.CreateEntity()}, m_sName{name}, m_sGroup{group}
	{
		AddComponent<Identification>(Identification{
				.name = name,
				.group = group,
				.entity_id = static_cast<int32_t>(m_Entity)
			});
	}
}