#pragma once
#include "GPButtons.h"
#include "Button.h"
#include <ScionUtilities/SDL_Wrappers.h>

namespace SCION_WINDOWING::Inputs
{
class Gamepad
{
  public:
	Gamepad( Controller controller );
	~Gamepad() = default;

	void Update();
	void OnBtnPressed( int btn );
	void OnBtnReleased( int btn );

	const bool IsBtnPressed( int btn ) const;
	const bool IsBtnJustPressed( int btn ) const;
	const bool IsBtnJustReleased( int btn ) const;

	const bool IsGamepadPresent() const;

	const Sint16 GetAxisPosition( Uint8 axis );
	void SetAxisPositionValue( Uint8 axis, Sint16 value );

	inline void SetJoystickHatValue( Uint8 value ) { m_JoystickHatValue = value; }
	inline const Uint8 GetJoystickHatValue() const { return m_JoystickHatValue; }

	inline const bool CheckJoystickID( SDL_JoystickID joystickID ) const { return m_InstanceID == joystickID; }
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
