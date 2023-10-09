#include "Button.h"

void SCION_WINDOWING::Inputs::Button::Update(bool bPressed)
{
	bJustPressed = !bIsPressed && bPressed;
	bJustReleased = bIsPressed && !bPressed;
	bIsPressed = bPressed;
}

void SCION_WINDOWING::Inputs::Button::Reset()
{
	bJustPressed = false;
	bJustReleased = false;
}

