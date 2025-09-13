#pragma once
#include "GPButtons.h"
#include "Button.h"
#include <ScionUtilities/SDL_Wrappers.h>

namespace SCION_WINDOWING::Inputs
{
class Gamepad
{
  public:
	/**
	 * @brief Constructs a Gamepad object from an SDL Controller.
	 *
	 * Initializes button maps, axis values, joystick details, and logs device info.
	 * Throws if the controller or joystick is invalid.
	 *
	 * @param controller SDL2 controller handle (RAII wrapped).
	 */
	explicit Gamepad( Controller controller );
	~Gamepad() = default;

	/**
	 * @brief Updates the gamepad state for a new frame.
	 *
	 * Resets all button states so "just pressed" and "just released" flags can be refreshed.
	 */
	void Update();

	/**
	 * @brief Handles a button press event.
	 *
	 * Updates the state of the specified button to pressed.
	 *
	 * @param btn Button identifier.
	 */
	void OnBtnPressed( int btn );

	/**
	 * @brief Handles a button release event.
	 *
	 * Updates the state of the specified button to released.
	 *
	 * @param btn Button identifier.
	 */
	void OnBtnReleased( int btn );

	/**
	 * @brief Checks if a button is currently being held down.
	 *
	 * @param btn Button identifier.
	 * @return true if the button is pressed, false otherwise.
	 */
	const bool IsBtnPressed( int btn ) const;

	/**
	 * @brief Checks if a button was just pressed this frame.
	 *
	 * @param btn Button identifier.
	 * @return true if the button transitioned from released to pressed, false otherwise.
	 */
	const bool IsBtnJustPressed( int btn ) const;

	/**
	 * @brief Checks if a button was just released this frame.
	 *
	 * @param btn Button identifier.
	 * @return true if the button transitioned from pressed to released, false otherwise.
	 */
	const bool IsBtnJustReleased( int btn ) const;

	/**
	 * @brief Checks if the gamepad is present and available.
	 *
	 * @return true if the controller exists and at least one joystick is connected.
	 */
	const bool IsGamepadPresent() const;

	const bool IsRumbleSupported() const;

	/**
	 * @brief Retrieves the current position of an axis.
	 *
	 * @param axis Axis identifier (0–5 typical for standard controllers).
	 * @return Axis value, or 0 if axis does not exist.
	 */
	const Sint16 GetAxisPosition( Uint8 axis );

	/**
	 * @brief Sets the raw position value of an axis.
	 *
	 * Typically called when processing SDL axis motion events.
	 *
	 * @param axis Axis identifier.
	 * @param value Axis value (range usually -32768 to 32767).
	 */
	void SetAxisPositionValue( Uint8 axis, Sint16 value );

	/**
	 * @brief Triggers the gamepad's rumble (vibration) feature.
	 *
	 * @param lowFrequencyRumble Strength of low-frequency motor (0–65535).
	 * @param highFrequencyRumble Strength of high-frequency motor (0–65535).
	 * @param durationMs Duration of rumble in milliseconds.
	 */
	void RumbleController( Uint16 lowFrequencyRumble, Uint16 highFrequencyRumble, Uint32 durationMs );

	/**
	 * @brief Sets the current joystick hat (D-pad) value.
	 *
	 * The hat value represents the directional state of the D-pad
	 * (e.g., centered, up, down, left, right, or diagonal).
	 *
	 * @param value SDL hat value (e.g., SCION_HAT_CENTERED, SDL_HAT_UP, etc.).
	 */
	inline void SetJoystickHatValue( Uint8 value ) { m_JoystickHatValue = value; }

	/**
	 * @brief Retrieves the current joystick hat (D-pad) value.
	 *
	 * @return The SDL hat value representing the current D-pad state.
	 */
	inline const Uint8 GetJoystickHatValue() const { return m_JoystickHatValue; }

	/**
	 * @brief Checks if a given joystick ID matches this gamepad instance.
	 *
	 * Useful for validating whether an SDL event belongs to this specific controller.
	 *
	 * @param joystickID SDL joystick instance ID to compare.
	 * @return true if the ID matches this gamepad, false otherwise.
	 */
	inline const bool CheckJoystickID( SDL_JoystickID joystickID ) const { return m_InstanceID == joystickID; }

	/**
	 * @brief Retrieves the name of the gamepad.
	 *
	 * @return A reference to the string containing the gamepad's name.
	 */
	inline const std::string& GetName() const { return m_sName; }

  private:
	Controller m_pController;
	std::map<int, Button> m_mapButtons;
	SDL_JoystickID m_InstanceID;
	std::map<Uint8, Sint16> m_mapAxisValues;
	Uint8 m_JoystickHatValue;
	std::string m_sName;
};
} // namespace SCION_WINDOWING::Inputs
