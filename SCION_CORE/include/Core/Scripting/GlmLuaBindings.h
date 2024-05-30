#pragma once
#include <sol/sol.hpp>

namespace SCION_CORE::Scripting
{
struct GLMBindings
{
	static void CreateGLMBindings(sol::state& lua);
};
} // namespace SCION_CORE::Scripting