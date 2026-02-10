#include "Core/Scripting/ContactListenerBind.h"
#include <Physics/ContactListener.h>
#include "Core/ECS/MetaUtilities.h"
#include <Physics/UserData.h>
#include <Logger/Logger.h>

using namespace Scion::Core::Utils;

namespace Scion::Core::Scripting
{

std::tuple<sol::object, sol::object> ContactListenerBinder::GetUserData(
	Scion::Physics::ContactListener& contactListener, sol::this_state s )
{
	auto pUserDataA = contactListener.GetUserDataA();
	auto pUserDataB = contactListener.GetUserDataB();

	if ( !pUserDataA || !pUserDataB )
		return std::make_tuple( sol::lua_nil_t{}, sol::lua_nil_t{} );

	SCION_ASSERT( pUserDataA->type_id != 0 && pUserDataB->type_id != 0 && "User Data Type id must be set!" );

	using namespace entt::literals;

	const auto maybe_any_a =
		InvokeMetaFunction( static_cast<entt::id_type>( pUserDataA->type_id ), "get_user_data"_hs, *pUserDataA, s );

	const auto maybe_any_b =
		InvokeMetaFunction( static_cast<entt::id_type>( pUserDataB->type_id ), "get_user_data"_hs, *pUserDataB, s );

	if ( !maybe_any_a || !maybe_any_b )
		return std::make_tuple( sol::lua_nil_t{}, sol::lua_nil_t{} );

	return std::make_tuple( maybe_any_a.cast<sol::reference>(), maybe_any_b.cast<sol::reference>() );
}

void ContactListenerBinder::CreateLuaContactListener( sol::state& lua, entt::registry& registry )
{
	auto& contactListener = registry.ctx().get<std::shared_ptr<Scion::Physics::ContactListener>>();
	if ( !contactListener )
	{
		SCION_ERROR( "Failed to create the contact listener lua bind - Contact listener is not in the registry!" );
		return;
	}

	lua.new_usertype<Scion::Physics::ContactListener>(
		"ContactListener", sol::no_constructor, "getUserData", [ & ]( sol::this_state s ) {
			return GetUserData( *contactListener, s );
		} );
}
} // namespace Scion::Core::Scripting
