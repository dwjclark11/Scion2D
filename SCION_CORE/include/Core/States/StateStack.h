#pragma once
#include <vector>
#include "State.h"

namespace SCION_CORE
{
class StateStack
{
  public:
	StateStack() = default;
	~StateStack() = default;

	void Push( State& state );
	void Pop();
	void ChangeState( State& state );
	void RemoveState( const std::string& sState );
	void Update( const float dt );
	void Render();

	State& Top();

	static void CreateLuaStateStackBind( sol::state& lua );

  private:
	std::vector<State> m_States{};
	std::unique_ptr<State> m_pStateHolder{ nullptr };
};
} // namespace SCION_CORE
