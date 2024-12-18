#pragma once
#include <sol/sol.hpp>

namespace SCION_CORE
{
namespace ECS
{
class Registry;
}
} // namespace SCION_CORE

namespace SCION_CORE::Scripting
{
struct RendererBinder
{
	static void CreateRenderingBind( sol::state& lua, SCION_CORE::ECS::Registry& registry );
};
} // namespace SCION_CORE::Scripting
