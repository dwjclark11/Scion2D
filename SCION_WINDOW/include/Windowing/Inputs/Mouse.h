#pragma once
#include "Button.h"
#include "MouseButtons.h"

namespace Scion::Windowing::Inputs
{
class Mouse
{
  public:
	Mouse() = default;
	~Mouse() = default;

	void Update();
	void OnBtnPressed( int btn );
	void OnBtnReleased( int btn );

	const bool IsBtnPressed( int btn ) const;
	const bool IsBtnJustPressed( int btn ) const;
	const bool IsBtnJustReleased( int btn ) const;

	const std::tuple<float, float> GetMouseScreenPosition();

	inline void SetMouseWheelX( float wheel ) { m_WheelX = wheel; }
	inline void SetMouseWheelY( float wheel ) { m_WheelY = wheel; }
	inline void SetMouseMoving( bool moving ) { m_bMouseMoving = moving; }

	inline const float GetMouseWheelX() const { return m_WheelX; }
	inline const float GetMouseWheelY() const { return m_WheelY; }
	inline const bool IsMouseMoving() const { return m_bMouseMoving; }

  private:
	std::map<int, Button> m_mapButtons{
		{ SCION_MOUSE_LEFT, Button{} },
		{ SCION_MOUSE_MIDDLE, Button{} },
		{ SCION_MOUSE_RIGHT, Button{} },
	};

	float m_X{ 0 };
	float m_Y{ 0 };
	float m_WheelX{ 0 };
	float m_WheelY{ 0 };
	bool m_bMouseMoving{ false };
};
} // namespace Scion::Windowing::Inputs
