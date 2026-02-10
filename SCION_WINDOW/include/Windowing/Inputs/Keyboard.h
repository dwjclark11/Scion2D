#pragma once
#include "Keys.h"
#include "Button.h"

namespace Scion::Windowing::Inputs
{
class Keyboard
{
  public:
	Keyboard();
	~Keyboard() = default;

	/**
	 * @brief Updates the keyboard state for a new frame.
	 *
	 * Resets all key states so "just pressed" and "just released" flags can be refreshed.
	 */
	void Update();

	/**
	 * @brief Handles a key press event.
	 *
	 * Updates the state of the specified key to pressed.
	 *
	 * @param key Key identifier.
	 */
	void OnKeyPressed( int key );

	/**
	 * @brief Handles a key release event.
	 *
	 * Updates the state of the specified key to released.
	 *
	 * @param key Key identifier.
	 */
	void OnKeyReleased( int key );

	/**
	 * @brief Checks if a key is currently being held down.
	 *
	 * @param key Key identifier.
	 * @return true if the key is pressed, false otherwise.
	 */
	const bool IsKeyPressed( int key ) const;

	/**
	 * @brief Checks if a key was just pressed this frame.
	 *
	 * @param key Key identifier.
	 * @return true if the key transitioned from released to pressed, false otherwise.
	 */
	const bool IsKeyJustPressed( int key ) const;

	/**
	 * @brief Checks if a key was just released this frame.
	 *
	 * @param key Key identifier.
	 * @return true if the key transitioned from pressed to released, false otherwise.
	 */
	const bool IsKeyJustReleased( int key ) const;

	const std::map<int, Button>& GetButtonMap() const { return m_mapButtons; }

  private:
	std::map<int, Button> m_mapButtons;
};
} // namespace Scion::Windowing::Inputs
