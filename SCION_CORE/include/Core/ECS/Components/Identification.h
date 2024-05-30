#pragma once
#include <string>

namespace SCION_CORE::ECS
{
struct Identification
{
	std::string name{"GameObject"}, group{""};
	int32_t entity_id{-1};
};
} // namespace SCION_CORE::ECS
