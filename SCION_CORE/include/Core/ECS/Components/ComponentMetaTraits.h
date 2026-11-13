#pragma once
#include <cstdint>
#include <entt/entt.hpp>

namespace Scion::Core::ECS
{
// Must be an enum, used as a bitmask with .traits()
enum class EMetaTraits : uint32_t
{
	None = 0,
	IsComponent = 1 << 0,
};

// Separate struct for the category/order data, stored via .custom()
struct ComponentMetaInfo
{
	const char* sCategory{ "General" };
	int32_t nOrder{ 999 };
};

} // namespace Scion::Core::ECS

// Required so entt's operator& works on EMetaTraits
template <>
struct entt::enum_as_bitmask<Scion::Core::ECS::EMetaTraits> : std::true_type
{
};
