#pragma once
#include "State.h"

#include <map>

namespace SCION_CORE
{
class StateMachine
{
  public:
	StateMachine();
	StateMachine( const sol::table& stateFuncs );

	void ChangeState( const std::string& stateName, bool bRemoveState = false,
					  const sol::object& enterParams = sol::lua_nil_t{} );

	void Update( const float dt );
	void Render();
	void AddState( const State& state );
	void ExitState();
	void DestroyStates();
	const std::string& CurrentState() const { return m_sCurrentState; }

	static void CreateLuaStateMachine( sol::state& lua );

  private:
	std::map<std::string, std::shared_ptr<State>> m_mapStates;
	std::string m_sCurrentState;
	std::optional<sol::table> m_StateTable;
};
} // namespace SCION_CORE
