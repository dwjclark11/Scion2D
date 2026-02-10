#include "ScionUtilities/Timer.h"
#include <iostream>

namespace Scion::Utilities
{

void Timer::Start()
{
	if ( !m_bIsRunning )
	{
		m_StartPoint = steady_clock::now();
		m_bIsRunning = true;
		m_bIsPaused = false;
	}
}

void Timer::Stop()
{
	if ( m_bIsRunning )
		m_bIsRunning = false;
}

void Timer::Pause()
{
	if ( m_bIsRunning && !m_bIsPaused )
	{
		m_bIsPaused = true;
		m_PausedPoint = steady_clock::now();
	}
}

void Timer::Resume()
{
	if ( m_bIsRunning && m_bIsPaused )
	{
		m_bIsPaused = false;
		m_StartPoint += duration_cast<milliseconds>( steady_clock::now() - m_PausedPoint );
	}
}

const int64_t Timer::ElapsedMS()
{
	if ( m_bIsRunning )
	{
		if ( m_bIsPaused )
			return duration_cast<milliseconds>( m_PausedPoint - m_StartPoint ).count();
		else
			return duration_cast<milliseconds>( steady_clock::now() - m_StartPoint ).count();
	}

	return 0;
}

const int64_t Timer::ElapsedSec()
{
	return ElapsedMS() / 1000;
}
} // namespace Scion::Utilities
