#pragma once
#include <sol/sol.hpp>
#include <entt/entt.hpp>

namespace SCION_PHYSICS 
{
class ContactListener;
}

namespace SCION_CORE::Scripting
{
class ContactListenerBinder
{
  private:
	static std::tuple<sol::object, sol::object> GetUserData( SCION_PHYSICS::ContactListener& contactListener,
															 sol::this_state s );

  public:
	static void CreateLuaContactListener( sol::state& lua, entt::registry& registry );
};
} // namespace SCION_CORE::Scripting
