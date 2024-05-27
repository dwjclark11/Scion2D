#include "Core/States/StateStack.h"
#include <Logger/Logger.h>

namespace SCION_CORE {

	void StateStack::Push(State& state)
	{
		auto hasState = std::find_if(m_States.begin(), m_States.end(),
			[&](const auto& s) { return s.name == state.name; }
		);

		if (hasState == m_States.end())
		{
			state.bAddState = true;
			m_pStateHolder = std::make_unique<State>(state);
			return;
		}

		SCION_ASSERT(false && "Trying to add a state that is already in the stack!");
	}

	void StateStack::Pop()
	{
		if (m_States.empty())
		{
			SCION_ERROR("Trying to pop an empty state stack!");
			return;
		}

		auto& top = m_States.back();
		top.bKillState = true;
	}

	void StateStack::ChangeState(State& state)
	{
		if (!m_States.empty())
			Pop();

		Push(state);
	}

	void StateStack::Update(const float dt)
	{
		if (m_pStateHolder && m_pStateHolder->bAddState)
		{
			m_States.push_back(*m_pStateHolder);

			if (m_pStateHolder->on_enter.valid())
			{
				try
				{
					auto result = m_pStateHolder->on_enter();
					if (!result.valid())
					{
						sol::error error = result;
						throw error;
					}
				}
				catch (const sol::error& error)
				{
					SCION_ERROR(error.what());
				}

				m_pStateHolder = nullptr;
			}
		}

		if (m_States.empty())
			return;

		auto& topState = m_States.back();
		if (topState.on_update.valid())
		{
			try
			{
				auto result = topState.on_update(dt);
				if (!result.valid())
				{
					sol::error error = result;
					throw error;
				}
			}
			catch (const sol::error& error)
			{
				SCION_ERROR("Failed to update top state: {}", error.what());
				return;
			}
			catch (const std::exception& ex)
			{
				SCION_ERROR("Failed to update top state: {}", ex.what());
				return;
			}
			catch (...)
			{
				SCION_ERROR("Failed to update top state: Unknown Error");
				return;
			}
		}

		if (topState.handle_inputs.valid())
		{
			try
			{
				auto result = topState.handle_inputs();
				if (!result.valid())
				{
					sol::error error = result;
					throw error;
				}
			}
			catch (const sol::error& error)
			{
				SCION_ERROR("Failed to handle top state's inputs: {}", error.what());
				return;
			}
		}

		if (topState.bKillState)
		{
			if (topState.on_exit.valid())
			{
				try
				{
					auto result = topState.on_exit();
					if (!result.valid())
					{
						sol::error error = result;
						throw error;
					}
				}
				catch (const sol::error& error)
				{
					SCION_ERROR("Failed to exit top state: {}", error.what());
					return;
				}
			}

			m_States.pop_back();
		}

	}

	void StateStack::Render()
	{
		for (const auto& state : m_States)
		{
			if (state.on_render.valid())
			{
				try
				{
					auto result = state.on_render();
					if (!result.valid())
					{
						sol::error error = result;
						throw error;
					}
				}
				catch (const sol::error& error)
				{
					SCION_ERROR("Failed to render state [{}]: {}", state.name, error.what());
				}
			}
		}
	}

	State& StateStack::Top()
	{
		SCION_ASSERT(!m_States.empty() && "Cannot get the top of an empty stack");

		if (m_States.empty())
			throw std::runtime_error("State stack is empty!");

		return m_States.back();
	}

	void StateStack::CreateLuaStateStackBind(sol::state& lua)
	{
		lua.new_usertype<StateStack>(
			"StateStack",
			sol::call_constructor,
			sol::constructors<StateStack()>(),
			"change_state", &StateStack::ChangeState,
			"push", &StateStack::Push,
			"pop", &StateStack::Pop,
			"update", &StateStack::Update,
			"render", &StateStack::Render,
			"top", &StateStack::Top
		);
	}
}

