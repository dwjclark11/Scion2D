#pragma once
#include <sol/sol.hpp>

namespace Scion::Core::Scripting
{
struct LuaFilesystem
{
	static void CreateLuaFileSystemBind( sol::state& lua );
};
} // namespace Scion::Core::Scripting
