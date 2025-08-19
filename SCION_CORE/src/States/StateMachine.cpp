#include "Core/States/StateMachine.h"
#include "Core/States/State.h"
#include "Logger/Logger.h"

namespace SCION_CORE
{
StateMachine::StateMachine()
	: StateMachine( sol::lua_nil_t{} )
{
}

StateMachine::StateMachine( const sol::table& stateFuncs )
	: m_mapStates{}
	, m_sCurrentState{}
	, m_StateTable{ stateFuncs }
{
}

void StateMachine::ChangeState( const std::string& stateName, bool bRemoveState, const sol::object& enterParams )
{
	auto stateItr = m_mapStates.find( stateName );
	if ( stateItr == m_mapStates.end() )
	{
		SCION_ERROR( "Failed to change states. [{}] does not exist.", stateName );
		return;
	}

	auto& newState = stateItr->second;

	if ( m_sCurrentState.empty() )
	{
		m_sCurrentState = stateName;
	}
	else
	{
		auto& oldState = m_mapStates.at( m_sCurrentState );
		if ( oldState->on_exit.valid() )
		{
			try
			{
				auto result = oldState->on_exit( oldState->variables );
				if ( !result.valid() )
				{
					sol::error error = result;
					throw error;
				}
			}
			catch ( const sol::error& error )
			{
				SCION_ERROR( "Failed to exit state: {}", error.what() );
				return;
			}
		}

		if ( bRemoveState )
			oldState->bKillState;

		m_sCurrentState = stateName;
	}

	if ( newState->on_enter.valid() )
	{
		try
		{
			auto result = newState->on_enter( newState->variables, enterParams );
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}
		catch ( const sol::error& error )
		{
			SCION_ERROR( "Failed to enter state: {}", error.what() );
			return;
		}
	}
}

void StateMachine::Update( const float dt )
{
	try
	{
		auto stateItr = m_mapStates.find( m_sCurrentState );
		if ( stateItr == m_mapStates.end() )
			return;

		auto& stateRef = stateItr->second;

		if ( stateRef->handle_inputs.valid() )
		{
			auto result = stateRef->handle_inputs( stateRef->variables );
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}

		if ( stateRef->on_update.valid() )
		{
			auto result = stateRef->on_update( stateRef->variables, dt );
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}

		// Check for states to remove
		std::erase_if( m_mapStates, []( auto& state ) { return state.second->bKillState; } );
	}
	catch ( const sol::error& error )
	{
		SCION_ERROR( "Failed to update state: {}", error.what() );
	}
	catch ( ... )
	{
		SCION_ERROR( "Failed to update state: Unknown error." );
	}
}

void StateMachine::Render()
{
	try
	{
		auto stateItr = m_mapStates.find( m_sCurrentState );
		if ( stateItr == m_mapStates.end() )
			return;

		auto& stateRef = stateItr->second;

		if ( stateRef->on_render.valid() )
		{
			auto result = stateRef->on_render( stateRef->variables );
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}
	}
	catch ( const sol::error& error )
	{
		SCION_ERROR( "Failed to render state: {}", error.what() );
	}
	catch ( ... )
	{
		SCION_ERROR( "Failed to render state: Unknown error." );
	}
}

void StateMachine::AddState( const State& state )
{
	if ( m_mapStates.contains( state.sName ) )
	{
		SCION_ERROR( "Failed to add state: {} -- Already exists.", state.sName );
		return;
	}

	m_mapStates.emplace( state.sName, std::make_shared<State>( state ) );
}

void StateMachine::ExitState()
{
	auto stateItr = m_mapStates.find( m_sCurrentState );
	if ( stateItr == m_mapStates.end() )
	{
		SCION_ERROR( "Failed to exit state: {} -- State does not exist.", m_sCurrentState );
		return;
	}
	auto& stateRef = stateItr->second;
	stateRef->on_exit( stateRef->variables );
	stateRef->bKillState = true;
	m_sCurrentState.clear();
}

void StateMachine::DestroyStates()
{
	for ( auto& [ name, state ] : m_mapStates )
	{
		state->on_exit( state->variables );
	}

	m_mapStates.clear();
}

void StateMachine::CreateLuaStateMachine( sol::state& lua )
{
	lua.new_usertype<StateMachine>(
		"StateMachine",
		sol::call_constructor,
		sol::constructors<StateMachine(), StateMachine( const sol::table& )>(),
		"changeState",
		sol::overload(
			[]( StateMachine& sm, const std::string& state, bool bRemove, const sol::object& enterParams ) {
				sm.ChangeState( state, bRemove, enterParams );
			},
			[]( StateMachine& sm, const std::string& state, bool bRemove ) { sm.ChangeState( state, bRemove ); },
			[]( StateMachine& sm, const std::string& state ) { sm.ChangeState( state ); } ),
		"update",
		&StateMachine::Update,
		"render",
		&StateMachine::Render,
		"currentState",
		&StateMachine::CurrentState,
		"addState",
		&StateMachine::AddState,
		"exitState",
		&StateMachine::ExitState,
		"destroy",
		&StateMachine::DestroyStates );
}
} // namespace SCION_CORE
