#pragma once
#include <sol/sol.hpp>

namespace Scion::Core
{
namespace ECS
{
class Registry;
}
} // namespace Scion::Core

namespace Scion::Core::Scripting
{
struct RendererBinder
{
	static void CreateRenderingBind( sol::state& lua, Scion::Core::ECS::Registry& registry );
};
} // namespace Scion::Core::Scripting
