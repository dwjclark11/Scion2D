
# SCION LOGGER 
There are currently two types of loggers. 

## Logger Class
* For logging errors, warnings, and info.
* This can be initialized to log to the console or to be used only in editor, or both.
  
### Logger Class Functions
* Use fmt::format under the hood and can take variadic args.
  
* SCION_LOG
```cpp
std::string sInfo{"Info here"};
SCION_LOG("This is some info: {}", sInfo);
```

* SCION_ERROR
```cpp
std::string sError{"Error here"};
SCION_ERROR("This is some Error: {}", sError);
```

## Crash Logger Class
* The crash logger binds to error signals so we can get the file and line where we crashed in c++.
* If there is an active lua state that is valid, it will also log the current lua stack trace.
* This will log to the console and to a crash_logs file.

### Crash Logger Setup
* In order to use the Crash Logger it must be setup.
* There are some helper macros:
  *  ```SCION_INIT_CRASH_LOGS()```
    * This will grab the Crash Logger instance and call it's Initialize function.
    * This should be called during your application initialization, but after the regular logger is initialized.
    * ```cpp
      bool Aplication::Initialize() { SCION_INIT_LOGS(true, true); SCION_INIT_CRASH_LOGS(); /*More Here*/ }
      ```
    * In order to log the lua stack trace, you need to set the ```Lua_State*``` in the CrashLogger.
    * This needs to be done before you actually run your scene.
    * ```cpp
      // Set the lua state for the crash logger.
	  // This is used to log the lua stack trace in case of a crash
      // You need to grab the underlying Lua_State* from the sol::state
	  SCION_CRASH_LOGGER().SetLuaState( lua->lua_state() );
      ```
  * There are some test bindings that you can also setup to ensure that the crash logger is working for you.
  * Grab the ```CrashLoggerTests::CreateLuaBind(sol::state& lua)``` function and bind the test logic to lua.
  * Run this script in your **main.lua** file:
  * ```lua
    main = { 
	  [1] = { 
		  update = function() 
			  -- Call this function to Crash the game and test 
			  -- The CrashLogger
			  S2D_CrashTest_Start()
		  end
	  }, 
	  [2] = { 
		  render = function() end
	  }, 
    }
    ```

* From there the application should crash and it should show you were it crash in C++ and also the latest lua call stack.
