#include "Core/Scripting/CrashLoggerTestBindings.h"
#include "Core/ECS/MainRegistry.h"
#include "Logger/CrashLogger.h"

namespace Scion::Core::Scripting
{
static void FunctionC()
{
	// Crash happens here
	int* ptr{ nullptr };
	*ptr = 194;
}

static void FunctionB()
{
	FunctionC();
}

static void FunctionA()
{
	FunctionB();
}

// Bind this function to Lua
static void CrashFromLua()
{
	FunctionA();
}

void CrashLoggerTests::CreateLuaBind( sol::state& lua )
{
	lua_State* L = lua.lua_state();
	SCION_CRASH_LOGGER().SetLuaState( L );

	lua.set_function( "S2D_CrashLoggerTest", &CrashFromLua );

	lua.script( R"(
        function S2D_CrashTest_level5()
            print("Entering Level 5...")
			-- Calls the C++ function that crashes
            S2D_CrashLoggerTest()  
        end

        function S2D_CrashTest_level4()
            print("Entering Level 4...")
            S2D_CrashTest_level5()
        end

        function S2D_CrashTest_level3()
            print("Entering Level 3...")
            S2D_CrashTest_level4()
        end

        function S2D_CrashTest_level2()
            print("Entering Level 2...")
            S2D_CrashTest_level3()
        end

        function S2D_CrashTest_level1()
            print("Entering Level 1...")
            S2D_CrashTest_level2()
        end

        function S2D_CrashTest_Start()
            print("Starting Lua Crash Test Execution...")
            S2D_CrashTest_level1()
        end
    )" );
}
} // namespace Scion::Core::Scripting
