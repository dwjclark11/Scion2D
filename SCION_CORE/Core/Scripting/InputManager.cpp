#include "InputManager.h"

namespace SCION_CORE {

    InputManager::InputManager()
        : m_pKeyboard{ std::make_unique<Keyboard>() }
    {
        //m_pKeyboard = std::make_unique<Keyboard>();
    }

	void InputManager::RegisterLuaKeyNames(sol::state& lua)
	{
        // ==================================================================
      // Register Typewriter Keys
      // ==================================================================
        lua.set("KEY_A", SCION_KEY_A);
        lua.set("KEY_B", SCION_KEY_B);
        lua.set("KEY_C", SCION_KEY_C);
        lua.set("KEY_D", SCION_KEY_D);
        lua.set("KEY_E", SCION_KEY_E);
        lua.set("KEY_F", SCION_KEY_F);
        lua.set("KEY_G", SCION_KEY_G);
        lua.set("KEY_H", SCION_KEY_H);
        lua.set("KEY_I", SCION_KEY_I);
        lua.set("KEY_J", SCION_KEY_J);
        lua.set("KEY_K", SCION_KEY_K);
        lua.set("KEY_L", SCION_KEY_L);
        lua.set("KEY_M", SCION_KEY_M);
        lua.set("KEY_N", SCION_KEY_N);
        lua.set("KEY_O", SCION_KEY_O);
        lua.set("KEY_P", SCION_KEY_P);
        lua.set("KEY_Q", SCION_KEY_Q);
        lua.set("KEY_R", SCION_KEY_R);
        lua.set("KEY_S", SCION_KEY_S);
        lua.set("KEY_T", SCION_KEY_T);
        lua.set("KEY_U", SCION_KEY_U);
        lua.set("KEY_V", SCION_KEY_V);
        lua.set("KEY_W", SCION_KEY_W);
        lua.set("KEY_X", SCION_KEY_X);
        lua.set("KEY_Y", SCION_KEY_Y);
        lua.set("KEY_Z", SCION_KEY_Z);

        lua.set("KEY_0", SCION_KEY_0);
        lua.set("KEY_1", SCION_KEY_1);
        lua.set("KEY_2", SCION_KEY_2);
        lua.set("KEY_3", SCION_KEY_3);
        lua.set("KEY_4", SCION_KEY_4);
        lua.set("KEY_5", SCION_KEY_5);
        lua.set("KEY_6", SCION_KEY_6);
        lua.set("KEY_7", SCION_KEY_7);
        lua.set("KEY_8", SCION_KEY_8);
        lua.set("KEY_9", SCION_KEY_9);

        lua.set("KEY_ENTER", SCION_KEY_RETURN);
        lua.set("KEY_BACKSPACE", SCION_KEY_BACKSPACE);
        lua.set("KEY_ESC", SCION_KEY_ESCAPE);
        lua.set("KEY_SPACE", SCION_KEY_SPACE);
        lua.set("KEY_LCTRL", SCION_KEY_LCTRL);
        lua.set("KEY_RCTRL", SCION_KEY_RCTRL);
        lua.set("KEY_LALT", SCION_KEY_LALT);
        lua.set("KEY_RALT", SCION_KEY_RALT);
        lua.set("KEY_LSHIFT", SCION_KEY_LSHIFT);
        lua.set("KEY_RSHIFT", SCION_KEY_RSHIFT);

        // ==================================================================
        //  Register Function Keys
        // ==================================================================
        lua.set("KEY_F1", SCION_KEY_F1);
        lua.set("KEY_F2", SCION_KEY_F2);
        lua.set("KEY_F3", SCION_KEY_F3);
        lua.set("KEY_F4", SCION_KEY_F4);
        lua.set("KEY_F5", SCION_KEY_F5);
        lua.set("KEY_F6", SCION_KEY_F6);
        lua.set("KEY_F7", SCION_KEY_F7);
        lua.set("KEY_F8", SCION_KEY_F8);
        lua.set("KEY_F9", SCION_KEY_F9);
        lua.set("KEY_F10", SCION_KEY_F10);
        lua.set("KEY_F11", SCION_KEY_F11);
        lua.set("KEY_F12", SCION_KEY_F12);

        // ==================================================================
        // Register Cursor Control Keys
        // ==================================================================
        lua.set("KEY_UP", SCION_KEY_UP);
        lua.set("KEY_RIGHT", SCION_KEY_RIGHT);
        lua.set("KEY_DOWN", SCION_KEY_DOWN);
        lua.set("KEY_LEFT", SCION_KEY_LEFT);

        // ==================================================================
        // Register Numeric Keypad Keys
        // ==================================================================
        lua.set("KP_KEY_0", SCION_KEY_KP0);
        lua.set("KP_KEY_1", SCION_KEY_KP1);
        lua.set("KP_KEY_2", SCION_KEY_KP2);
        lua.set("KP_KEY_3", SCION_KEY_KP3);
        lua.set("KP_KEY_4", SCION_KEY_KP4);
        lua.set("KP_KEY_5", SCION_KEY_KP5);
        lua.set("KP_KEY_6", SCION_KEY_KP6);
        lua.set("KP_KEY_7", SCION_KEY_KP7);
        lua.set("KP_KEY_8", SCION_KEY_KP8);
        lua.set("KP_KEY_9", SCION_KEY_KP9);
        lua.set("KP_KEY_ENTER", SCION_KEY_KP_ENTER);
	}

	InputManager& InputManager::GetInstance()
	{
		static InputManager instance{};
		return instance;
	}

	void InputManager::CreateLuaInputBindings(sol::state& lua)
	{
        RegisterLuaKeyNames(lua);
        
        auto& inputManager = GetInstance();
        auto& keyboard = inputManager.GetKeyboard();

        lua.new_usertype<Keyboard>(
            "Keyboard",
            sol::no_constructor,
            "just_pressed", [&](int key) { return keyboard.IsKeyJustPressed(key); },
            "just_released", [&](int key) { return keyboard.IsKeyJustReleased(key); },
            "pressed", [&](int key) { return keyboard.IsKeyPressed(key); }
        );
	}
}