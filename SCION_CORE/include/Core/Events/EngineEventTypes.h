#pragma once
#include <sol/sol.hpp>
#include "Physics/UserData.h"

namespace SCION_CORE::Events
{
struct ContactEvent
{
	SCION_PHYSICS::ObjectData objectA{};
	SCION_PHYSICS::ObjectData objectB{};
};

enum class EKeyEventType
{
	Pressed,
	Released,
	NoType
};

struct KeyEvent
{
	int key{ -1 };
	EKeyEventType eType{ EKeyEventType::NoType };
};

enum class EGamepadConnectType
{
	Connected,
	Disconnected,
	NotConnected
};

struct GamepadConnectEvent
{
	EGamepadConnectType eConnectType{ EGamepadConnectType::NotConnected };
	int index{ 1 };
};

struct LuaEvent
{
	sol::object data{ sol::lua_nil };
};

struct LuaEventBinder
{
	static void CreateLuaEventBindings( sol::state& lua );
};

} // namespace SCION_CORE::Events
