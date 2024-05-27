#include "Core/Scripting/UserDataBindings.h"
#include <Logger/Logger.h>
#include "Core/ECS/MetaUtilities.h"

using namespace SCION_CORE::Utils;
using namespace SCION_PHYSICS;

void SCION_CORE::Scripting::UserDataBinder::CreateLuaUserData(sol::state& lua)
{
	using namespace entt::literals;

	lua.new_usertype<UserData>(
		"UserData",
		"type_id", entt::type_hash<UserData>::value,
		"create_user_data", [](const sol::table& data, sol::this_state s) -> sol::object {
			if (!data.valid())
			{
				SCION_ASSERT(false && "User data provided is an invalid type");
				return sol::lua_nil_t{};
			}

			const auto maybe_any = InvokeMetaFunction(
				GetIdType(data),
				"create_user_data"_hs,
				data, s
			);

			return maybe_any ? maybe_any.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"set_user_data", [](UserData& userData, const sol::table& data, sol::this_state s) -> sol::object {
			if (!data.valid())
			{
				SCION_ASSERT(false && "User data provided is an invalid type");
				return sol::lua_nil_t{};
			}

			const auto maybe_any = InvokeMetaFunction(
				GetIdType(data),
				"set_user_data"_hs,
				&userData, data, s
			);

			return maybe_any ? maybe_any.cast<sol::reference>() : sol::lua_nil_t{};
		},
		"get_user_data", [](UserData& userData, sol::this_state s) {
			const auto maybe_any = InvokeMetaFunction(
				static_cast<entt::id_type>(userData.type_id),
				"get_user_data"_hs,
				userData, s
			);

			return maybe_any ? maybe_any.cast<sol::reference>() : sol::lua_nil_t{};
		}
	);
}
