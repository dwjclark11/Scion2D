#pragma once
#include "Keys.h"
#include "Button.h"
#include <map>

namespace SCION_WINDOWING::Inputs
{
class Keyboard
{
  private:
	std::map<int, Button> m_mapButtons;

  public:
	Keyboard();
	~Keyboard() = default;

	void Update();
	void OnKeyPressed( int key );
	void OnKeyReleased( int key );

	const bool IsKeyPressed( int key ) const;
	const bool IsKeyJustPressed( int key ) const;
	const bool IsKeyJustReleased( int key ) const;

	const std::map<int, Button>& GetButtonMap() const { return m_mapButtons; }
};
} // namespace SCION_WINDOWING::Inputs
