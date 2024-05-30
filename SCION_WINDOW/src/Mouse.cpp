#include "Windowing/Inputs/Mouse.h"
#include <SDL.h>
#include <Logger/Logger.h>

namespace SCION_WINDOWING::Inputs
{

void Mouse::Update()
{
	for (auto& [ btn, button ] : m_mapButtons)
		button.Reset();

	m_WheelX = 0;
	m_WheelY = 0;
	m_bMouseMoving = false;
}

void Mouse::OnBtnPressed(int btn)
{
	if (btn == SCION_MOUSE_UNKNOWN)
	{
		SCION_ERROR("Mouse Button [{}] is unknown!", btn);
		return;
	}

	auto btnItr = m_mapButtons.find(btn);
	if (btnItr == m_mapButtons.end())
	{
		SCION_ERROR("Mouse Button [{}] does not exist!", btn);
		return;
	}

	btnItr->second.Update(true);
}

void Mouse::OnBtnReleased(int btn)
{
	if (btn == SCION_MOUSE_UNKNOWN)
	{
		SCION_ERROR("Mouse Button [{}] is unknown!", btn);
		return;
	}

	auto btnItr = m_mapButtons.find(btn);
	if (btnItr == m_mapButtons.end())
	{
		SCION_ERROR("Mouse Button [{}] does not exist!", btn);
		return;
	}

	btnItr->second.Update(false);
}

const bool Mouse::IsBtnPressed(int btn) const
{
	if (btn == SCION_MOUSE_UNKNOWN)
	{
		SCION_ERROR("Mouse Button [{}] is unknown!", btn);
		return false;
	}

	auto btnItr = m_mapButtons.find(btn);
	if (btnItr == m_mapButtons.end())
	{
		SCION_ERROR("Mouse Button [{}] does not exist!", btn);
		return false;
	}

	return btnItr->second.bIsPressed;
}

const bool Mouse::IsBtnJustPressed(int btn) const
{
	if (btn == SCION_MOUSE_UNKNOWN)
	{
		SCION_ERROR("Mouse Button [{}] is unknown!", btn);
		return false;
	}

	auto btnItr = m_mapButtons.find(btn);
	if (btnItr == m_mapButtons.end())
	{
		SCION_ERROR("Mouse Button [{}] does not exist!", btn);
		return false;
	}

	return btnItr->second.bJustPressed;
}

const bool Mouse::IsBtnJustReleased(int btn) const
{
	if (btn == SCION_MOUSE_UNKNOWN)
	{
		SCION_ERROR("Mouse Button [{}] is unknown!", btn);
		return false;
	}

	auto btnItr = m_mapButtons.find(btn);
	if (btnItr == m_mapButtons.end())
	{
		SCION_ERROR("Mouse Button [{}] does not exist!", btn);
		return false;
	}

	return btnItr->second.bJustReleased;
}

const std::tuple<int, int> Mouse::GetMouseScreenPosition()
{
	SDL_GetMouseState(&m_X, &m_Y);
	return std::make_tuple(m_X, m_Y);
}
} // namespace SCION_WINDOWING::Inputs