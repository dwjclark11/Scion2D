#pragma once
#include <entt/entt.hpp>

namespace SCION_CORE::Events
{
/*
 * EventDispatcher
 * Simple wrapper around entt::dispatcher.
 * Not all functionality has been wrapped, only what is currently used.
 * May add more to this later as needed.
 */
class EventDispatcher
{
  public:
	EventDispatcher() = default;
	~EventDispatcher() = default;

	/*
	 * @brief Add an event handler with specified function.
	 * The function must take in the event type as an argument.
	 * @tparam TEventType - The type of event.
	 * @tparam Func - Function to be used.
	 * @tparam THandlerType - The actual handler object passed in.
	 * @param A valid handler object for the function provided.
	 */
	template <typename TEventType, auto Func, typename THandlerType>
	void AddHandler( THandlerType& handler )
	{
		m_Dispatcher.sink<TEventType>().connect<Func>( handler );
	}

	/*
	 * @brief Remove an event handler with specified function.
	 * The function must take in the event type as an argument.
	 * @tparam TEventType - The type of event.
	 * @tparam Func - Function to be used.
	 * @tparam THandlerType - The actual handler object passed in.
	 * @param A valid handler object to be removed.
	 */
	template <typename TEventType, auto Func, typename THandlerType>
	void RemoveHandler( THandlerType& handler )
	{
		m_Dispatcher.sink<TEventType>().disconnect<Func>( handler );
	}

	/*
	 * @brief Clear all handlers of specified event type.
	 * @tparam The type of event to clear handlers.
	 */
	template <typename TEventType>
	void ClearHandlers()
	{
		m_Dispatcher.sink<TEventType>().disconnect();
	}

	/*
	 * @brief Add free function to receive events.
	 * The function must take in the event type as an argument.
	 * @tparam TEventType - The type of event.
	 * @tparam Func - Function to be used.
	 */
	template <typename TEventType, auto Func>
	void AddFreeFunc()
	{
		m_Dispatcher.sink<TEventType>().connect<Func>();
	}

	template <typename TEventType>
	void EmitEvent( TEventType& ev )
	{
		m_Dispatcher.trigger( ev );
	}

	template <typename TEventType>
	void EmitEvent( TEventType&& ev )
	{
		m_Dispatcher.trigger( ev );
	}

	template <typename TEventType>
	void EnqueueEvent( TEventType& ev )
	{
		m_Dispatcher.enqueue<TEventType>( ev );
	}

	template <typename TEventType>
	void EnqueueEvent( TEventType&& ev )
	{
		m_Dispatcher.enqueue<TEventType>( ev );
	}

	/*
	 * @brief Update or deliver all pending events of a given queue.
	 * @tparam TEventType The type of event to update/send.
	 */
	template <typename TEventType>
	void UpdateEvent()
	{
		m_Dispatcher.update<TEventType>();
	}

	/*
	 * @brief Update or deliver all pending events that have been queued.
	 */
	void UpdateAll() { m_Dispatcher.update(); }

  private:
	entt::dispatcher m_Dispatcher;
};

} // namespace SCION_CORE::Events
