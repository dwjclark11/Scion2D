#pragma once
#include <sol/sol.hpp>
#include <entt/entt.hpp>

namespace Scion::Physics 
{
class ContactListener;
}

namespace Scion::Core::Scripting
{
class ContactListenerBinder
{
  private:
	static std::tuple<sol::object, sol::object> GetUserData( Scion::Physics::ContactListener& contactListener,
															 sol::this_state s );

  public:
	static void CreateLuaContactListener( sol::state& lua, entt::registry& registry );
};
} // namespace Scion::Core::Scripting
