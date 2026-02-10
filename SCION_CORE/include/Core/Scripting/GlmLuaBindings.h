#pragma once
#include <sol/sol.hpp>

namespace Scion::Core::Scripting
{
struct GLMBindings
{
	static void CreateGLMBindings( sol::state& lua );
};
} // namespace Scion::Core::Scripting
