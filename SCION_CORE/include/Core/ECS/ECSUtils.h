#pragma once
#include <string>
#include <entt/entt.hpp>

namespace SCION_CORE::ECS
{
class Registry;

entt::entity FindEntityByTag( Registry& registry, const std::string& sTag );

} // namespace SCION_CORE::ECS
