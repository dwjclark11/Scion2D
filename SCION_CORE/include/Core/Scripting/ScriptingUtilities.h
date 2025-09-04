#pragma once
#include <sol/sol.hpp>
#include <stdexcept>
#include <initializer_list>
#include <utility>

namespace SCION_CORE::Scripting
{

struct MainScriptFunctions
{
	sol::protected_function init{ sol::lua_nil };
	sol::protected_function update{ sol::lua_nil };
	sol::protected_function render{ sol::lua_nil };
};

/*
 * @brief Helper function to create and bind ReadOnly tables in lua.
 * All keys and values must be of the same type.
 * @tparam TKey type of the key in the table.
 * @tparam TValue type of the value
 * @return Returns the created readonly sol::table
 */
template <typename TKey, typename TValue>
sol::table MakeReadOnlyTable( sol::state_view lua, std::initializer_list<std::pair<TKey, TValue>> entries )
{
	// Create the table
	sol::table tbl = lua.create_table();

	sol::table metaTbl = lua.create_table_with();

	// Add the necessary entries to the meta table, not the original table
	for ( const auto& [ key, value ] : entries )
	{
		metaTbl[ key ] = value;
	}
	metaTbl[ sol::meta_function::new_index ] = []( sol::this_state s ) {
		return luaL_error( s, "Attempt to modify read-only table" );
	};
	metaTbl[ sol::meta_function::index ] = metaTbl;

	// Optional: protect the metatable from being tampered with
	metaTbl.set_function( "__metatable", [] { return "This metatable is locked."; } );

	// Set the metatable on the table
	tbl[ sol::metatable_key ] = metaTbl;

	return tbl;
}

/*
 * @brief Helper function to create and bind ReadOnly tables in lua.
 * The values are automatically deduced using the type-erased sol::object.
 * Use std::make_object(this_state, value)
 * @tparam TKey type of the key in the table.
 * @return Returns the created readonly sol::table
 */
template <typename TKey>
sol::table MakeReadOnlyTable( sol::state_view lua, std::initializer_list<std::pair<TKey, sol::object>> entries )
{
	// Create the table
	sol::table tbl = lua.create_table();

	// Create a metatable that blocks all assignments
	sol::table metaTbl = lua.create_table_with();

	// Add the necessary entries to the meta table, not the original table
	for ( const auto& [ key, value ] : entries )
	{
		metaTbl[ key ] = value;
	}

	metaTbl[ sol::meta_function::new_index ] = []( sol::this_state s ) {
		return luaL_error( s, "Attempt to modify read-only table" );
	};
	metaTbl[ sol::meta_function::index ] = metaTbl;

	// Optional: protect the metatable from being tampered with
	metaTbl.set_function( "__metatable", [] { return "This metatable is locked."; } );

	// Set the metatable on the table
	tbl[ sol::metatable_key ] = metaTbl;

	return tbl;
}

struct ScriptingHelpers
{
	static void CreateLuaHelpers( sol::state& lua );
};

} // namespace SCION_CORE::Scripting

using MainScriptPtr = std::shared_ptr<SCION_CORE::Scripting::MainScriptFunctions>;
