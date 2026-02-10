#pragma once
#include <entt/entt.hpp>

namespace Scion::Core::ECS
{
class Registry;

entt::entity FindEntityByTag( Registry& registry, const std::string& sTag );

} // namespace Scion::Core::ECS
