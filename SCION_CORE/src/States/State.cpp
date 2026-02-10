#include "Core/States/State.h"
#include <Logger/Logger.h>

void Scion::Core::State::CreateLuaStateBind( sol::state& lua )
{
	lua.new_usertype<State>(
		"State",
		sol::call_constructor,
		sol::factories( []( const std::string& name ) { return State{ .sName = name }; },
						[]( const std::string& name,
							sol::protected_function on_enter,
							sol::protected_function on_exit,
							sol::protected_function on_update,
							sol::protected_function on_render,
							sol::protected_function handle_inputs,
							sol::object variables ) {
							return State{ .sName = name,
										  .on_render = on_render,
										  .on_update = on_update,
										  .on_enter = on_enter,
										  .on_exit = on_exit,
										  .handle_inputs = handle_inputs,
										  .variables = variables };
						} ),
		"setOnEnter",
		[]( State& state, sol::protected_function on_enter ) {
			if ( !on_enter.valid() )
			{
				SCION_ERROR( "Failed to set the on_enter function" );
				return;
			}
			state.on_enter = on_enter;
		},
		"setOnExit",
		[]( State& state, sol::protected_function on_exit ) {
			if ( !on_exit.valid() )
			{
				SCION_ERROR( "Failed to set the on_exit function" );
				return;
			}
			state.on_exit = on_exit;
		},
		"setOnUpdate",
		[]( State& state, sol::protected_function on_update ) {
			if ( !on_update.valid() )
			{
				SCION_ERROR( "Failed to set the on_update function" );
				return;
			}
			state.on_update = on_update;
		},
		"setOnRender",
		[]( State& state, sol::protected_function on_render ) {
			if ( !on_render.valid() )
			{
				SCION_ERROR( "Failed to set the on_render function" );
				return;
			}
			state.on_render = on_render;
		},
		"setHandleInputs",
		[]( State& state, sol::protected_function handle_inputs ) {
			if ( !handle_inputs.valid() )
			{
				SCION_ERROR( "Failed to set the handle_inputs function" );
				return;
			}
			state.handle_inputs = handle_inputs;
		},
		"setVariableTable",
		[]( State& state, const sol::table& table ) {
			if ( !table.valid() )
			{
				SCION_ERROR( "Failed to set the variables function" );
				return;
			}
			state.variables = table;
		},
		"variables",
		&State::variables,
		"name",
		&State::sName,
		"bKillState",
		&State::bKillState );
}
