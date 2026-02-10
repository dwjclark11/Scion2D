#include "EventDispatcher.h"
#include "Logger/Logger.h"

namespace Scion::Core::Events
{

template <typename TEventType, auto Func, typename THandlerType>
auto EventDispatcher::AddHandler( THandlerType& handler )
{
	return m_pDispatcher->sink<TEventType>().template connect<Func>( handler );
}

template <typename TEventType, auto Func, typename THandlerType>
void EventDispatcher::RemoveHandler( THandlerType& handler )
{
	m_pDispatcher->sink<TEventType>().template disconnect<Func>( handler );
}

template <typename TEventType>
void EventDispatcher::ClearHandlers()
{
	m_pDispatcher->sink<TEventType>().disconnect();
}

template <typename TEventType>
inline bool EventDispatcher::HasHandlers()
{
	return !m_pDispatcher->sink<TEventType>().empty();
}

template <typename TEventType, auto Func>
void EventDispatcher::AddFreeFunc()
{
	m_pDispatcher->sink<TEventType>().template connect<Func>();
}

template <typename TEventType>
void EventDispatcher::EmitEvent( TEventType& ev )
{
	m_pDispatcher->trigger( ev );
}

template <typename TEventType>
void EventDispatcher::EmitEvent( TEventType&& ev )
{
	m_pDispatcher->trigger( ev );
}

template <typename TEventType>
void EventDispatcher::EnqueueEvent( TEventType& ev )
{
	m_pDispatcher->enqueue<TEventType>( ev );
}

template <typename TEventType>
void EventDispatcher::EnqueueEvent( TEventType&& ev )
{
	m_pDispatcher->enqueue<TEventType>( ev );
}

template <typename TEventType>
void EventDispatcher::UpdateEvent()
{
	m_pDispatcher->update<TEventType>();
}

template <typename TEvent>
void add_handler( EventDispatcher& dispatcher, const sol::table& handler /*, LuaHandler<TEvent>& handler */ )
{
	if ( !handler.valid() )
	{
		SCION_ERROR( "Failed to add new event handler. Handler was invalid." );
		return;
	}

	auto* handleRef{ handler.as<LuaHandler<TEvent>*>() };
	handleRef->connection = dispatcher.AddHandler<TEvent, &LuaHandler<TEvent>::HandleEvent>( *handleRef );
}

template <typename TEvent>
void remove_handler( EventDispatcher& dispatcher, const sol::table& handler )
{
	if ( !handler.valid() )
	{
		SCION_ERROR( "Failed to add remove handler. Handler was invalid." );
		return;
	}

	dispatcher.RemoveHandler<TEvent, &LuaHandler<TEvent>::HandleEvent>( handler.as<LuaHandler<TEvent>>() );
}

template <typename TEvent>
void emit_event( EventDispatcher& dispatcher, const sol::table& event )
{
	if ( !event.valid() )
	{
		SCION_ERROR( "Failed to emit event. Event was invalid." );
		return;
	}

	dispatcher.EmitEvent<TEvent>( event.as<TEvent>() );
}

template <typename TEvent>
void enqueue_event( EventDispatcher& dispatcher, const sol::table& event )
{
	if ( !event.valid() )
	{
		SCION_ERROR( "Failed to enqueue event. Event was invalid." );
		return;
	}

	dispatcher.EnqueueEvent<TEvent>( event.as<TEvent>() );
}

template <typename TEvent>
void update_event( EventDispatcher& dispatcher )
{
	dispatcher.UpdateEvent<TEvent>();
}

template <typename TEvent>
bool has_handlers( EventDispatcher& dispatcher )
{
	return dispatcher.HasHandlers<TEvent>();
}

template <typename TEvent>
inline void EventDispatcher::RegisterMetaEventFuncs()
{
	using namespace entt::literals;
	entt::meta_factory<TEvent>()
		.type( entt::type_hash<TEvent>::value() )
		.template func<&add_handler<TEvent>>( "add_handler"_hs )
		.template func<&remove_handler<TEvent>>( "remove_handler"_hs )
		.template func<&emit_event<TEvent>>( "emit_event"_hs )
		.template func<&enqueue_event<TEvent>>( "enqueue_event"_hs )
		.template func<&update_event<TEvent>>( "update_event"_hs )
		.template func<&has_handlers<TEvent>>( "has_handlers"_hs );
}

} // namespace Scion::Core::Events
