#include "Core/Scripting/InputManager.h"
#include <Windowing/Inputs/Keyboard.h>
#include <Windowing/Inputs/Mouse.h>
#include <Windowing/Inputs/Gamepad.h>

#include <Logger/Logger.h>
#include <ScionUtilities/SDL_Wrappers.h>
#include <glm/glm.hpp>

#include <Rendering/Core/Camera2D.h>

using namespace SCION_WINDOWING::Inputs;

namespace SCION_CORE
{

InputManager::InputManager()
	: m_pKeyboard{ std::make_unique<Keyboard>() }
	, m_pMouse{ std::make_unique<Mouse>() }
{
	// Load already connected devices
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		SDL_GameController* pController = SDL_GameControllerOpen( i );
		if (pController)
		{
			std::shared_ptr<Gamepad> gamepad{ nullptr };
			try
			{
				gamepad = std::make_shared<Gamepad>( std::move( MakeSharedFromSDLType<Controller>( pController ) ) );
			}
			catch ( ... )
			{
				std::string error{ SDL_GetError() };
				SCION_ERROR( "Failed to Open gamepad device -- {}", error );
				continue;
			}

			for ( int j = 1; j <= MAX_CONTROLLERS; j++ )
			{
				if ( m_mapGameControllers.contains( j ) )
					continue;

				m_mapGameControllers.emplace( j, std::move( gamepad ) );
				SCION_LOG( "Gamepad [{}] was added at index [{}]", i, j );
				break;
			}
		}
	}
}

void InputManager::RegisterLuaKeyNames( sol::state& lua )
{
	// ==================================================================
	// Register Typewriter Keys
	// ==================================================================
	lua.set( "KEY_A", SCION_KEY_A );
	lua.set( "KEY_B", SCION_KEY_B );
	lua.set( "KEY_C", SCION_KEY_C );
	lua.set( "KEY_D", SCION_KEY_D );
	lua.set( "KEY_E", SCION_KEY_E );
	lua.set( "KEY_F", SCION_KEY_F );
	lua.set( "KEY_G", SCION_KEY_G );
	lua.set( "KEY_H", SCION_KEY_H );
	lua.set( "KEY_I", SCION_KEY_I );
	lua.set( "KEY_J", SCION_KEY_J );
	lua.set( "KEY_K", SCION_KEY_K );
	lua.set( "KEY_L", SCION_KEY_L );
	lua.set( "KEY_M", SCION_KEY_M );
	lua.set( "KEY_N", SCION_KEY_N );
	lua.set( "KEY_O", SCION_KEY_O );
	lua.set( "KEY_P", SCION_KEY_P );
	lua.set( "KEY_Q", SCION_KEY_Q );
	lua.set( "KEY_R", SCION_KEY_R );
	lua.set( "KEY_S", SCION_KEY_S );
	lua.set( "KEY_T", SCION_KEY_T );
	lua.set( "KEY_U", SCION_KEY_U );
	lua.set( "KEY_V", SCION_KEY_V );
	lua.set( "KEY_W", SCION_KEY_W );
	lua.set( "KEY_X", SCION_KEY_X );
	lua.set( "KEY_Y", SCION_KEY_Y );
	lua.set( "KEY_Z", SCION_KEY_Z );

	lua.set( "KEY_0", SCION_KEY_0 );
	lua.set( "KEY_1", SCION_KEY_1 );
	lua.set( "KEY_2", SCION_KEY_2 );
	lua.set( "KEY_3", SCION_KEY_3 );
	lua.set( "KEY_4", SCION_KEY_4 );
	lua.set( "KEY_5", SCION_KEY_5 );
	lua.set( "KEY_6", SCION_KEY_6 );
	lua.set( "KEY_7", SCION_KEY_7 );
	lua.set( "KEY_8", SCION_KEY_8 );
	lua.set( "KEY_9", SCION_KEY_9 );

	lua.set( "KEY_ENTER", SCION_KEY_RETURN );
	lua.set( "KEY_BACKSPACE", SCION_KEY_BACKSPACE );
	lua.set( "KEY_ESC", SCION_KEY_ESCAPE );
	lua.set( "KEY_SPACE", SCION_KEY_SPACE );
	lua.set( "KEY_LCTRL", SCION_KEY_LCTRL );
	lua.set( "KEY_RCTRL", SCION_KEY_RCTRL );
	lua.set( "KEY_LALT", SCION_KEY_LALT );
	lua.set( "KEY_RALT", SCION_KEY_RALT );
	lua.set( "KEY_LSHIFT", SCION_KEY_LSHIFT );
	lua.set( "KEY_RSHIFT", SCION_KEY_RSHIFT );

	// ==================================================================
	//  Register Punctuation Keys
	// ==================================================================
	lua.set( "KEY_COLON", SCION_KEY_COLON );
	lua.set( "KEY_SEMICOLON", SCION_KEY_SEMICOLON );
	lua.set( "KEY_QUOTE", SCION_KEY_QUOTE );
	lua.set( "KEY_BACKQUOTE", SCION_KEY_BACKQUOTE );
	lua.set( "KEY_CARET", SCION_KEY_CARET );
	lua.set( "KEY_UNDERSCORE", SCION_KEY_UNDERSCORE );
	lua.set( "KEY_RIGHTBRACKET", SCION_KEY_RIGHTBRACKET );
	lua.set( "KEY_LEFTBRACKET", SCION_KEY_LEFTBRACKET );
	lua.set( "KEY_SLASH", SCION_KEY_SLASH );
	lua.set( "KEY_ASTERISK", SCION_KEY_ASTERISK );
	lua.set( "KEY_LEFTPAREN", SCION_KEY_LEFTPAREN );
	lua.set( "KEY_RIGHTPAREN", SCION_KEY_RIGHTPAREN );
	lua.set( "KEY_QUESTION", SCION_KEY_QUESTION );
	lua.set( "KEY_AMPERSAND", SCION_KEY_AMPERSAND );
	lua.set( "KEY_DOLLAR", SCION_KEY_DOLLAR );
	lua.set( "KEY_EXCLAIM", SCION_KEY_EXCLAIM );
	lua.set( "KEY_BACKSLASH", SCION_KEY_BACKSLASH );

	// ==================================================================
	//  Register Function Keys
	// ==================================================================
	lua.set( "KEY_F1", SCION_KEY_F1 );
	lua.set( "KEY_F2", SCION_KEY_F2 );
	lua.set( "KEY_F3", SCION_KEY_F3 );
	lua.set( "KEY_F4", SCION_KEY_F4 );
	lua.set( "KEY_F5", SCION_KEY_F5 );
	lua.set( "KEY_F6", SCION_KEY_F6 );
	lua.set( "KEY_F7", SCION_KEY_F7 );
	lua.set( "KEY_F8", SCION_KEY_F8 );
	lua.set( "KEY_F9", SCION_KEY_F9 );
	lua.set( "KEY_F10", SCION_KEY_F10 );
	lua.set( "KEY_F11", SCION_KEY_F11 );
	lua.set( "KEY_F12", SCION_KEY_F12 );

	// ==================================================================
	// Register Cursor Control Keys
	// ==================================================================
	lua.set( "KEY_UP", SCION_KEY_UP );
	lua.set( "KEY_RIGHT", SCION_KEY_RIGHT );
	lua.set( "KEY_DOWN", SCION_KEY_DOWN );
	lua.set( "KEY_LEFT", SCION_KEY_LEFT );

	// ==================================================================
	// Register Numeric Keypad Keys
	// ==================================================================
	lua.set( "KP_KEY_0", SCION_KEY_KP0 );
	lua.set( "KP_KEY_1", SCION_KEY_KP1 );
	lua.set( "KP_KEY_2", SCION_KEY_KP2 );
	lua.set( "KP_KEY_3", SCION_KEY_KP3 );
	lua.set( "KP_KEY_4", SCION_KEY_KP4 );
	lua.set( "KP_KEY_5", SCION_KEY_KP5 );
	lua.set( "KP_KEY_6", SCION_KEY_KP6 );
	lua.set( "KP_KEY_7", SCION_KEY_KP7 );
	lua.set( "KP_KEY_8", SCION_KEY_KP8 );
	lua.set( "KP_KEY_9", SCION_KEY_KP9 );

	lua.set( "KP_KEY_DIVIDE", SCION_KEY_KP_DIVIDE );
	lua.set( "KP_KEY_MULTIPLY", SCION_KEY_KP_MULTIPLY );
	lua.set( "KP_KEY_MINUS", SCION_KEY_KP_MINUS );
	lua.set( "KP_KEY_PLUS", SCION_KEY_KP_PLUS );
	lua.set( "KP_KEY_ENTER", SCION_KEY_KP_ENTER );
	lua.set( "KP_KEY_PERIOD", SCION_KEY_KP_PERIOD );

	lua.set( "KEY_NUM_LOCK", SCION_KEY_NUMLOCK );

	// Adding windows specific keys here for convenience
#ifdef _WIN32
	lua.set( "KEY_LWIN", SCION_KEY_LWIN );
	lua.set( "KEY_RWIN", SCION_KEY_RWIN );
#else
	lua.set( "KEY_LGUI", SCION_KEY_LGUI );
	lua.set( "KEY_RGUI", SCION_KEY_RGUI );
#endif
}

void InputManager::RegisterMouseBtnNames( sol::state& lua )
{
	lua.set( "LEFT_BTN", SCION_MOUSE_LEFT );
	lua.set( "MIDDLE_BTN", SCION_MOUSE_MIDDLE );
	lua.set( "RIGHT_BTN", SCION_MOUSE_RIGHT );
}

void InputManager::RegisterGamepadBtnNames( sol::state& lua )
{
	lua.set( "GP_BTN_A", SCION_GP_BTN_A );
	lua.set( "GP_BTN_B", SCION_GP_BTN_B );
	lua.set( "GP_BTN_X", SCION_GP_BTN_X );
	lua.set( "GP_BTN_Y", SCION_GP_BTN_Y );

	lua.set( "GP_BTN_BACK", SCION_GP_BTN_BACK );
	lua.set( "GP_BTN_GUIDE", SCION_GP_BTN_GUIDE );
	lua.set( "GP_BTN_START", SCION_GP_BTN_START );

	lua.set( "GP_LSTICK", SCION_GP_BTN_LSTICK );
	lua.set( "GP_RSTICK", SCION_GP_BTN_RSTICK );
	lua.set( "GP_LSHOULDER", SCION_GP_BTN_LSHOULDER );
	lua.set( "GP_RSHOULDER", SCION_GP_BTN_RSHOULDER );

	lua.set( "DPAD_UP", SCION_GP_BTN_DPAD_UP );
	lua.set( "DPAD_DOWN", SCION_GP_BTN_DPAD_DOWN );
	lua.set( "DPAD_LEFT", SCION_GP_BTN_DPAD_LEFT );
	lua.set( "DPAD_RIGHT", SCION_GP_BTN_DPAD_RIGHT );

	lua.set( "AXIS_X1", 0 );
	lua.set( "AXIS_Y1", 1 );
	lua.set( "AXIS_X2", 2 );
	lua.set( "AXIS_Y2", 3 );

	// Bottom triggers
	lua.set( "AXIS_Z1", 4 );
	lua.set( "AXIS_Z2", 5 );
}

InputManager& InputManager::GetInstance()
{
	static InputManager instance{};
	return instance;
}

void InputManager::CreateLuaInputBindings( sol::state& lua, SCION_CORE::ECS::Registry& registry )
{
	RegisterLuaKeyNames( lua );
	RegisterMouseBtnNames( lua );
	RegisterGamepadBtnNames( lua );

	auto& inputManager = GetInstance();
	auto& keyboard = inputManager.GetKeyboard();
	auto& camera = registry.GetContext<std::shared_ptr<SCION_RENDERING::Camera2D>>();

	lua.new_usertype<Keyboard>(
		"Keyboard",
		sol::no_constructor,
		"justPressed",
		[ & ]( int key ) { return keyboard.IsKeyJustPressed( key ); },
		"justReleased",
		[ & ]( int key ) { return keyboard.IsKeyJustReleased( key ); },
		"pressed",
		[ & ]( int key ) { return keyboard.IsKeyPressed( key ); },
		"pressedKeys",
		[ & ]() {
			std::vector<int> keys;
			for ( const auto& [ key, button ] : keyboard.GetButtonMap() )
			{
				if ( button.bIsPressed )
					keys.push_back( key );
			}
			return keys;
		} );

/*
 * In order for this to work in the editor, we need to take into account
 * the imgui window position, to get the world position.
 * There is new bindings inside the editor that handles this.
 */
#ifndef IN_SCION_EDITOR
	auto& mouse = inputManager.GetMouse();

	lua.new_usertype<Mouse>(
		"Mouse",
		sol::no_constructor,
		"justPressed",
		[ & ]( int btn ) { return mouse.IsBtnJustPressed( btn ); },
		"justReleased",
		[ & ]( int btn ) { return mouse.IsBtnJustReleased( btn ); },
		"pressed",
		[ & ]( int btn ) { return mouse.IsBtnPressed( btn ); },
		"screenPosition",
		[ & ]() {
			auto [ x, y ] = mouse.GetMouseScreenPosition();
			return glm::vec2{ x, y };
		},
		"worldPosition",
		[ & ]() {
			auto [ x, y ] = mouse.GetMouseScreenPosition();
			return camera->ScreenCoordsToWorld( glm::vec2{ x, y } );
		},
		"wheelX",
		[ & ]() { return mouse.GetMouseWheelX(); },
		"wheelY",
		[ & ]() { return mouse.GetMouseWheelY(); } );
#endif // IN_SCION_EDITOR

	lua.new_usertype<Gamepad>(
		"Gamepad",
		sol::no_constructor,
		"anyConnected",
		[ & ] { return inputManager.GamepadConnected(); },
		"connected",
		[ & ]( int index ) { return inputManager.GamepadConnected( index ); },
		"justPressed",
		[ & ]( int index, int btn ) {
			auto gamepad = inputManager.GetController( index );
			if ( !gamepad )
			{
				SCION_ERROR( "Invalid Gamepad index [{}] provided or gamepad is not plugged in!", index );
				return false;
			}
			return gamepad->IsBtnJustPressed( btn );
		},
		"justReleased",
		[ & ]( int index, int btn ) {
			auto gamepad = inputManager.GetController( index );
			if ( !gamepad )
			{
				SCION_ERROR( "Invalid Gamepad index [{}] provided or gamepad is not plugged in!", index );
				return false;
			}
			return gamepad->IsBtnJustReleased( btn );
		},
		"pressed",
		[ & ]( int index, int btn ) {
			auto gamepad = inputManager.GetController( index );
			if ( !gamepad )
			{
				SCION_ERROR( "Invalid Gamepad index [{}] provided or gamepad is not plugged in!", index );
				return false;
			}
			return gamepad->IsBtnPressed( btn );
		},
		"getAxisPosition",
		[ & ]( int index, int axis ) {
			auto gamepad = inputManager.GetController( index );
			if ( !gamepad )
			{
				SCION_ERROR( "Invalid Gamepad index [{}] provided or gamepad is not plugged in!", index );
				return Sint16{ 0 };
			}
			return gamepad->GetAxisPosition( axis );
		},
		"getHatValue",
		[ & ]( int index ) {
			auto gamepad = inputManager.GetController( index );
			if ( !gamepad )
			{
				SCION_ERROR( "Invalid Gamepad index [{}] provided or gamepad is not plugged in!", index );
				return Uint8{ 0 };
			}
			return gamepad->GetJoystickHatValue();
		} );
}

void InputManager::UpdateInputs()
{
	m_pKeyboard->Update();
	m_pMouse->Update();
	UpdateGamepads();
}

bool InputManager::GamepadConnected() const
{
	return std::ranges::any_of( m_mapGameControllers, []( const auto& pair ) { return pair.second != nullptr; } );
}

bool InputManager::GamepadConnected( int location ) const
{
	auto gamepadItr = m_mapGameControllers.find( location );
	return gamepadItr != m_mapGameControllers.end() && gamepadItr->second != nullptr;
}

std::shared_ptr<Gamepad> InputManager::GetController( int index )
{
	auto gamepadItr = m_mapGameControllers.find( index );
	if ( gamepadItr == m_mapGameControllers.end() )
	{
		SCION_ERROR( "Failed to get gamepad at index [{}] -- Does not exist!", index );
		return nullptr;
	}

	return gamepadItr->second;
}

int InputManager::AddGamepad( Sint32 gamepadIndex )
{
	// Trying to add a controller with the same id.
	if (std::ranges::any_of(m_mapGameControllers,
		[&gamepadIndex](const auto& pair) { return pair.second->CheckJoystickID(gamepadIndex); }))
	{
		SCION_WARN( "Trying to add a controller that is already mapped. Gamepad ID: {}", gamepadIndex );
		return -1;
	}

	if ( m_mapGameControllers.size() >= MAX_CONTROLLERS )
	{
		SCION_WARN( "Trying to add too many controllers! Max Controllers allowed = {}", MAX_CONTROLLERS );
		return -1;
	}

	std::shared_ptr<Gamepad> gamepad{ nullptr };
	try
	{
		gamepad = std::make_shared<Gamepad>(
			std::move( MakeSharedFromSDLType<Controller>( SDL_GameControllerOpen( gamepadIndex ) ) ) );
	}
	catch ( ... )
	{
		std::string error{ SDL_GetError() };
		SCION_ERROR( "Failed to Open gamepad device -- {}", error );
		return -1;
	}

	for ( int i = 1; i <= MAX_CONTROLLERS; i++ )
	{
		if ( m_mapGameControllers.contains( i ) )
			continue;

		m_mapGameControllers.emplace( i, std::move( gamepad ) );
		SCION_LOG( "Gamepad [{}] was added at index [{}]", gamepadIndex, i );
		return i;
	}

	SCION_ASSERT( false && "Failed to add the new controller!" );
	SCION_ERROR( "Failed to add the new controller!" );
	return -1;
}

int InputManager::RemoveGamepad( Sint32 gamepadID )
{
	auto gamepadItr = std::ranges::find_if(
		m_mapGameControllers, [ & ]( const auto& gamepad ) { return gamepad.second->CheckJoystickID( gamepadID ); } );

	int index{ -1 };
	if (gamepadItr == m_mapGameControllers.end())
	{
		SCION_ASSERT( false && "Failed to remove Gamepad must not have been mapped correctly." );
		SCION_ERROR( "Failed to remove Gamepad ID [{}] must not have been mapped correctly.", gamepadID );
		return index;	
	}

	index = gamepadItr->first;
	m_mapGameControllers.erase( gamepadItr );
	SCION_LOG( "Gamepad ID [{}] at index [{}] was removed", gamepadID, index );
	return index;
}

void InputManager::GamepadBtnPressed( const SDL_Event& event )
{
	for ( const auto& [ index, gamepad ] : m_mapGameControllers )
	{
		if ( gamepad && gamepad->CheckJoystickID( event.jdevice.which ) )
		{
			gamepad->OnBtnPressed( event.cbutton.button );
			break;
		}
	}
}

void InputManager::GamepadBtnReleased( const SDL_Event& event )
{
	for ( const auto& [ index, gamepad ] : m_mapGameControllers )
	{
		if ( gamepad && gamepad->CheckJoystickID( event.jdevice.which ) )
		{
			gamepad->OnBtnReleased( event.cbutton.button );
			break;
		}
	}
}
void InputManager::GamepadAxisValues( const SDL_Event& event )
{
	for ( const auto& [ index, gamepad ] : m_mapGameControllers )
	{
		if ( gamepad && gamepad->CheckJoystickID( event.jdevice.which ) )
		{
			gamepad->SetAxisPositionValue( event.jaxis.axis, event.jaxis.value );
			break;
		}
	}
}

void InputManager::GamepadHatValues( const SDL_Event& event )
{
	for ( const auto& [ index, gamepad ] : m_mapGameControllers )
	{
		if ( gamepad && gamepad->CheckJoystickID( event.jdevice.which ) )
		{
			gamepad->SetJoystickHatValue( event.jhat.value );
			break;
		}
	}
}

void InputManager::UpdateGamepads()
{
	for ( const auto& [ index, gamepad ] : m_mapGameControllers )
	{
		if ( gamepad )
			gamepad->Update();
	}
}
} // namespace SCION_CORE
