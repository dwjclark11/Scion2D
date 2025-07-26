#pragma once
#include <string>
#include <entt/entt.hpp>

namespace SCION_CORE::ECS
{
struct Identification
{
	/* Name identifier for the owner object. Eventually these will be unique. */
	std::string name{ "GameObject" };
	/* The group name that the owner belongs to. */
	std::string group{};
	/* The guid identifier for the entity. */
	std::string sGUID{};
	/* The underlying entity id of the owner. */
	uint32_t entity_id{ entt::null };
};
} // namespace SCION_CORE::ECS
