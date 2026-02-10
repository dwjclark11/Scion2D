#pragma once
#include <sol/sol.hpp>

namespace Scion::Core
{
namespace ECS
{
class Registry;
}
} // namespace Scion::Core

namespace Scion::Editor
{
struct LuaCoreBinder
{
	static void CreateLuaBind( sol::state& lua, Scion::Core::ECS::Registry& registry );
};
} // namespace Scion::Editor
