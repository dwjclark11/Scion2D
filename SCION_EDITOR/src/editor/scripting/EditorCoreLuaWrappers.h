#pragma once
#include <sol/sol.hpp>

namespace SCION_CORE
{
namespace ECS
{
class Registry;
}
} // namespace SCION_CORE

namespace SCION_EDITOR
{
struct LuaCoreBinder
{
	static void CreateLuaBind( sol::state& lua, SCION_CORE::ECS::Registry& registry );
};
} // namespace SCION_EDITOR
