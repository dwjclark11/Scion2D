#include "Windowing/Inputs/Keyboard.h"
#include <Logger/Logger.h>

namespace SCION_WINDOWING::Inputs
{

Keyboard::Keyboard()
	: m_mapButtons{ { SCION_KEY_BACKSPACE, Button{} },
					{ SCION_KEY_TAB, Button{} },
					{ SCION_KEY_CLEAR, Button{} },
					{ SCION_KEY_RETURN, Button{} },
					{ SCION_KEY_PAUSE, Button{} },
					{ SCION_KEY_ESCAPE, Button{} },
					{ SCION_KEY_SPACE, Button{} },
					{ SCION_KEY_EXCLAIM, Button{} },
					{ SCION_KEY_QUOTEDBL, Button{} },
					{ SCION_KEY_HASH, Button{} },
					{ SCION_KEY_DOLLAR, Button{} },
					{ SCION_KEY_AMPERSAND, Button{} },
					{ SCION_KEY_QUOTE, Button{} },
					{ SCION_KEY_LEFTPAREN, Button{} },
					{ SCION_KEY_RIGHTPAREN, Button{} },
					{ SCION_KEY_ASTERISK, Button{} },
					{ SCION_KEY_PLUS, Button{} },
					{ SCION_KEY_COMMA, Button{} },
					{ SCION_KEY_MINUS, Button{} },
					{ SCION_KEY_PERIOD, Button{} },
					{ SCION_KEY_SLASH, Button{} },
					{ SCION_KEY_0, Button{} },
					{ SCION_KEY_1, Button{} },
					{ SCION_KEY_2, Button{} },
					{ SCION_KEY_3, Button{} },
					{ SCION_KEY_4, Button{} },
					{ SCION_KEY_5, Button{} },
					{ SCION_KEY_6, Button{} },
					{ SCION_KEY_7, Button{} },
					{ SCION_KEY_8, Button{} },
					{ SCION_KEY_9, Button{} },
					{ SCION_KEY_COLON, Button{} },
					{ SCION_KEY_SEMICOLON, Button{} },
					{ SCION_KEY_LESS, Button{} },
					{ SCION_KEY_EQUALS, Button{} },
					{ SCION_KEY_GREATER, Button{} },
					{ SCION_KEY_QUESTION, Button{} },
					{ SCION_KEY_AT, Button{} },
					{ SCION_KEY_LEFTBRACKET, Button{} },
					{ SCION_KEY_BACKSLASH, Button{} },
					{ SCION_KEY_RIGHTBRACKET, Button{} },
					{ SCION_KEY_CARET, Button{} },
					{ SCION_KEY_UNDERSCORE, Button{} },
					{ SCION_KEY_BACKQUOTE, Button{} },
					{ SCION_KEY_A, Button{} },
					{ SCION_KEY_B, Button{} },
					{ SCION_KEY_C, Button{} },
					{ SCION_KEY_D, Button{} },
					{ SCION_KEY_E, Button{} },
					{ SCION_KEY_F, Button{} },
					{ SCION_KEY_G, Button{} },
					{ SCION_KEY_H, Button{} },
					{ SCION_KEY_I, Button{} },
					{ SCION_KEY_J, Button{} },
					{ SCION_KEY_K, Button{} },
					{ SCION_KEY_L, Button{} },
					{ SCION_KEY_M, Button{} },
					{ SCION_KEY_N, Button{} },
					{ SCION_KEY_O, Button{} },
					{ SCION_KEY_P, Button{} },
					{ SCION_KEY_Q, Button{} },
					{ SCION_KEY_R, Button{} },
					{ SCION_KEY_S, Button{} },
					{ SCION_KEY_T, Button{} },
					{ SCION_KEY_U, Button{} },
					{ SCION_KEY_V, Button{} },
					{ SCION_KEY_W, Button{} },
					{ SCION_KEY_X, Button{} },
					{ SCION_KEY_Y, Button{} },
					{ SCION_KEY_Z, Button{} },
					{ SCION_KEY_DELETE, Button{} },
					{ SCION_KEY_CAPSLOCK, Button{} },
					{ SCION_KEY_F1, Button{} },
					{ SCION_KEY_F2, Button{} },
					{ SCION_KEY_F3, Button{} },
					{ SCION_KEY_F4, Button{} },
					{ SCION_KEY_F5, Button{} },
					{ SCION_KEY_F6, Button{} },
					{ SCION_KEY_F7, Button{} },
					{ SCION_KEY_F8, Button{} },
					{ SCION_KEY_F9, Button{} },
					{ SCION_KEY_F10, Button{} },
					{ SCION_KEY_F11, Button{} },
					{ SCION_KEY_F12, Button{} },
					{ SCION_KEY_SCROLLOCK, Button{} },
					{ SCION_KEY_PRINTSCREEN, Button{} },
					{ SCION_KEY_INSERT, Button{} },
					{ SCION_KEY_HOME, Button{} },
					{ SCION_KEY_PAGEUP, Button{} },
					{ SCION_KEY_PAGEDOWN, Button{} },
					{ SCION_KEY_END, Button{} },
					{ SCION_KEY_RIGHT, Button{} },
					{ SCION_KEY_LEFT, Button{} },
					{ SCION_KEY_DOWN, Button{} },
					{ SCION_KEY_UP, Button{} },
					{ SCION_KEY_NUMLOCK, Button{} },
					{ SCION_KEY_KP_DIVIDE, Button{} },
					{ SCION_KEY_KP_MULTIPLY, Button{} },
					{ SCION_KEY_KP_MINUS, Button{} },
					{ SCION_KEY_KP_PLUS, Button{} },
					{ SCION_KEY_KP_ENTER, Button{} },
					{ SCION_KEY_KP1, Button{} },
					{ SCION_KEY_KP2, Button{} },
					{ SCION_KEY_KP3, Button{} },
					{ SCION_KEY_KP4, Button{} },
					{ SCION_KEY_KP5, Button{} },
					{ SCION_KEY_KP6, Button{} },
					{ SCION_KEY_KP7, Button{} },
					{ SCION_KEY_KP8, Button{} },
					{ SCION_KEY_KP9, Button{} },
					{ SCION_KEY_KP0, Button{} },
					{ SCION_KEY_KP_PERIOD, Button{} },
					{ SCION_KEY_LCTRL, Button{} },
					{ SCION_KEY_LSHIFT, Button{} },
					{ SCION_KEY_LALT, Button{} },
					{ SCION_KEY_RCTRL, Button{} },
					{ SCION_KEY_RSHIFT, Button{} },
					{ SCION_KEY_RALT, Button{} } }
{
	// Emplace the windows specific naming keys
	// This will only be for naming convenience,
	// Otherwise, we can just call it the name in SDL
#ifdef _WIN32
	m_mapButtons.emplace( SCION_KEY_LWIN, Button{} );
	m_mapButtons.emplace( SCION_KEY_RWIN, Button{} );
#else
	m_mapButtons.emplace( SCION_KEY_LGUI, Button{} );
	m_mapButtons.emplace( SCION_KEY_RGUI, Button{} );
#endif
}

void Keyboard::Update()
{
	for ( auto& [ key, button ] : m_mapButtons )
		button.Reset();
}

void Keyboard::OnKeyPressed( int key )
{
	if ( key == KEY_UNKNOWN )
	{
		SCION_ERROR( "Key [{}] is unknown!", key );
		return;
	}

	auto keyItr = m_mapButtons.find( key );
	if ( keyItr == m_mapButtons.end() )
	{
		SCION_ERROR( "Key [{}] does not exist!", key );
		return;
	}
	keyItr->second.Update( true );
}

void Keyboard::OnKeyReleased( int key )
{
	if ( key == KEY_UNKNOWN )
	{
		SCION_ERROR( "Key [{}] is unknown!", key );
		return;
	}

	auto keyItr = m_mapButtons.find( key );
	if ( keyItr == m_mapButtons.end() )
	{
		SCION_ERROR( "Key [{}] does not exist!", key );
		return;
	}
	keyItr->second.Update( false );
}

const bool Keyboard::IsKeyPressed( int key ) const
{
	if ( key == KEY_UNKNOWN )
	{
		SCION_ERROR( "Key [{}] is unknown!", key );
		return false;
	}

	auto keyItr = m_mapButtons.find( key );
	if ( keyItr == m_mapButtons.end() )
	{
		SCION_ERROR( "Key [{}] does not exist!", key );
		return false;
	}
	return keyItr->second.bIsPressed;
}

const bool Keyboard::IsKeyJustPressed( int key ) const
{
	if ( key == KEY_UNKNOWN )
	{
		SCION_ERROR( "Key [{}] is unknown!", key );
		return false;
	}

	auto keyItr = m_mapButtons.find( key );
	if ( keyItr == m_mapButtons.end() )
	{
		SCION_ERROR( "Key [{}] does not exist!", key );
		return false;
	}

	return keyItr->second.bJustPressed;
}

const bool Keyboard::IsKeyJustReleased( int key ) const
{

	if ( key == KEY_UNKNOWN )
	{
		SCION_ERROR( "Key [{}] is unknown!", key );
		return false;
	}

	auto keyItr = m_mapButtons.find( key );
	if ( keyItr == m_mapButtons.end() )
	{
		SCION_ERROR( "Key [{}] does not exist!", key );
		return false;
	}

	return keyItr->second.bJustReleased;
}
} // namespace SCION_WINDOWING::Inputs