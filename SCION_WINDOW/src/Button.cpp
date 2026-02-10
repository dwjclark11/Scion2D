#include "Windowing/Inputs/Button.h"

void Scion::Windowing::Inputs::Button::Update( bool bPressed )
{
	bJustPressed = !bIsPressed && bPressed;
	bJustReleased = bIsPressed && !bPressed;
	bIsPressed = bPressed;
}

void Scion::Windowing::Inputs::Button::Reset()
{
	bJustPressed = false;
	bJustReleased = false;
}
