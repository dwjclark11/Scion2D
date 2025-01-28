#pragma once
#include <sol/sol.hpp>

namespace SCION_CORE::Scripting
{
struct LuaFilesystem
{
	static void CreateLuaFileSystemBind( sol::state& lua );
};
} // namespace SCION_CORE::Scripting
