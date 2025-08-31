#include "Core/Events/EventDispatcher.h"
#include "Core/ECS/MetaUtilities.h"

using namespace SCION_CORE::Utils;

namespace SCION_CORE::Events
{
enum class EDispatcherType
{
	/* Used to create a new dispatcher in lua. */
	LUA_DISPATCHER,
	/* Use the event dispatcher that exists in engine. */
	SCION_DISPATCHER,
};

EventDispatcher::EventDispatcher()
	: m_pDispatcher{ std::make_shared<entt::dispatcher>() }
{
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::UpdateAll()
{
	m_pDispatcher->update();
}

void EventDispatcher::ClearQueue()
{
	m_pDispatcher->clear();
}

void EventDispatcher::CreateEventDispatcherLuaBind( sol::state& lua, EventDispatcher& dispatcher )
{
	using namespace entt::literals;

	lua.new_enum<EDispatcherType>( "DispatcherType",
								   {
									   { "Lua", EDispatcherType::LUA_DISPATCHER },
									   { "Scion", EDispatcherType::SCION_DISPATCHER },
								   } );

	lua.new_usertype<EventDispatcher>(
		"EventDispatcher",
		sol::call_constructor,
		sol::factories( [ & ]( EDispatcherType eType, sol::this_state s )
			{
			if ( eType == EDispatcherType::LUA_DISPATCHER )
				return EventDispatcher{};
			else
				return dispatcher;
		} ),
		"addHandler",
		[]( EventDispatcher& eventDispatcher, const sol::table& handler, const sol::object& type ) {
			const auto handle = InvokeMetaFunction( GetIdType( type ), "add_handler"_hs, eventDispatcher, handler );
		},
		"removeHandler",
		[]( EventDispatcher& eventDispatcher, const sol::table& handler, const sol::object& type ) {
			InvokeMetaFunction( GetIdType( type ), "remove_handler"_hs, eventDispatcher, handler );
		},
		"emitEvent",
		[]( EventDispatcher& eventDispatcher, const sol::table& event ) {
			const auto ev = InvokeMetaFunction( GetIdType( event ), "emit_event"_hs, eventDispatcher, event );
		},
		"enqueueEvent",
		[]( EventDispatcher& eventDispatcher, const sol::table& event ) {
			const auto ev = InvokeMetaFunction( GetIdType( event ), "enqueue_event"_hs, eventDispatcher, event );
		},
		"hasHandlers",
		[]( EventDispatcher& eventDispatcher, const sol::table& event ) {
			const auto has_handlers = InvokeMetaFunction( GetIdType( event ), "has_handlers"_hs, eventDispatcher );
			return has_handlers ? has_handlers.cast<bool>() : false;
		},
		"updateEvent",
		[]( EventDispatcher& eventDispatcher, const sol::table& event ) {
			const auto ev = InvokeMetaFunction( GetIdType( event ), "update_event"_hs, eventDispatcher );
		},
		"update",
		[]( EventDispatcher& eventDispatcher )
		{
			eventDispatcher.UpdateAll();
		},
		"clearQueue",
		[]( EventDispatcher& eventDispatcher ) { eventDispatcher.ClearQueue(); } );
}
} // namespace SCION_CORE::Events
