#pragma once
#include <sol/sol.hpp>

namespace SCION_CORE
{
struct State
{
	std::string name{ "" };
	sol::protected_function on_render{ sol::lua_nil };
	sol::protected_function on_update{ sol::lua_nil };
	sol::protected_function on_enter{ sol::lua_nil };
	sol::protected_function on_exit{ sol::lua_nil };
	sol::protected_function handle_inputs{ sol::lua_nil };

	sol::object variables{ sol::lua_nil };
	bool bKillState{ false };
	bool bAddState{ false };

	static void CreateLuaStateBind( sol::state& lua );
};
} // namespace SCION_CORE
