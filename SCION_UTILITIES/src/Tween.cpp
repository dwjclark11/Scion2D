#include "ScionUtilities/Tween.h"
#include <unordered_map>
#include <functional>
#include <cmath>

#include "Logger/Logger.h"
#include "ScionUtilities/MathUtilities.h"

namespace SCION_UTIL
{

/*
* These easing equations are based on the Easing.as functions by Robert Penner.
* http://robertpenner.com/easing/
*
* /*
	TERMS OF USE - EASING EQUATIONS
	---------------------------------------------------------------------------------
	Open source under the BSD License.

	Copyright © 2001 Robert Penner All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer. Redistributions in binary
	form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided
	with the distribution. Neither the name of the author nor the names of
	contributors may be used to endorse or promote products derived from this
	software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	---------------------------------------------------------------------------------
*/

/*
 * EasingFunction == float(float t, float b, float c, float d)
 * t: current time
 * b: beginning value
 * c: change in value
 * d: duration
 *
 * Returns the adjusted eased value based on the current time.
 */
using EasingFunction = std::function<float( float, float, float, float )>;

// clang-format off

// Linear has no easing. Just a linear change.
constexpr auto Linear =
[]( float current, float start, float change, float duration )
{
	return change * current / duration + start;
};

// Quadratic easing: t^2
constexpr auto EaseQuadIn = 
[]( float current, float start, float change, float duration )
{
	return change * (current /= duration) * current + start;
};

// Quadratic easing: t^2
constexpr auto EaseQuadOut = 
[]( float current, float start, float change, float duration )
{
	return -change * (current /= duration) * ( current - 2.f ) + start;
};

// Quadratic easing: t^2
constexpr auto EaseQuadInOut = 
[]( float current, float start, float change, float duration )
{
	if ((current /= duration/2.f) < 1.f)
		return change /2.f *current*current + start;

	return -change / 2.f * ((--current) * (current - 2.f) - 1.f) + start;
};

// Sinusodial easing: sin(t) 
constexpr auto SineIn =
[]( float current, float start, float change, float duration )
{
	return -change * sin((current / duration) * SCION_MATH::PIOver2) + change + start;
};

// Sinusodial easing: sin(t) 
constexpr auto SineOut =
[]( float current, float start, float change, float duration )
{
	return change * sin((current / duration) * SCION_MATH::PIOver2) + start;
};

// Sinusodial easing: sin(t) 
constexpr auto SineInOut =
[]( float current, float start, float change, float duration )
{
	return -change / 2.f * (cos(SCION_MATH::PI * current / duration) - 1.f) + start;
};

// Elastic easing: [IN] - Oscillates around the start position.
constexpr auto ElasticIn = 
[]( float current, float start, float change, float duration )
{
	if (current == 0.f)
		return start;
	if ((current /= duration) == 1.f)
		return start + change;

	float p = duration * 0.3f;
	float a = change;
	float s = p / 4.f;

	return -(a * powf(2.f, -10 * (current-=1)) * sin((current * duration - s) * SCION_MATH::TwoPI / p)) + start;
};

// Elastic easing: [OUT] - Oscillates around the end position.
constexpr auto ElasticOut =
[]( float current, float start, float change, float duration )
{
	if (current == 0.f)
		return start;
	if ((current /= duration) == 1.f)
		return start + change;

	float p = duration * 0.3f;
	float a = change;
	float s = p / 4.f;

	return -(a * powf(2.f, -10 * current) * sin((current * duration - s) * SCION_MATH::TwoPI / p)) + change + start;
};

// Elastic easing: [IN-OUT] - Oscillates around the end position.
constexpr auto ElasticInOut =
[]( float current, float start, float change, float duration )
{
	if (current == 0.f)
		return start;
	if ((current /= duration/2.f) == 2.f)
		return start + change;

	float p = duration * 0.3f * 1.5f;
	float a = change;
	float s = p / 4.f;

	if (current < 1.f)
		return -0.5f * (a * powf(2.f, -10 * (current-=1)) * sin((current * duration - s) * SCION_MATH::TwoPI / p)) + start;

	return (a * powf(2.f, -10 * (current -= 1.f)) * sin((current * duration - s) * SCION_MATH::TwoPI / p)) * 0.5f + change + start;
};

// Exponential easing: 2^t
constexpr auto ExponentialIn =
[](float current, float start, float change, float duration)
{
	if (current == 0.f)
		return start + change;

	return change * powf(2, -10 * (current / duration - 1.f)) + start;
};

// Exponential easing: 2^t
constexpr auto ExponentialOut =
[](float current, float start, float change, float duration)
{
	if (current == duration)
		return start + change;

	return change * (-powf(2, -10 * current / duration) + 1.f) + start;
};

// Exponential easing: 2^t
constexpr auto ExponentialInOut =
[](float current, float start, float change, float duration)
{
	if (current == 0.f)
		return start;

	if (current == duration)
		return start + change;

	if ((current /= duration / 2.f) < 1.f)
		return change / 2.f * powf(2, -10 * (current - 1.f)) + start;

	return change / 2.f * (-powf(2, -10 * --current) + 2.f) + start;
};

constexpr auto BounceIn = 
[](float current, float start, float change, float duration)
{
	SCION_ERROR("EASE_IN_BOUNCE, has not been implemented.");
	// TODO:
	return 0.f;
};

constexpr auto BounceOut =
[](float current, float start, float change, float duration)
{
	SCION_ERROR("EASE_OUT_BOUNCE, has not been implemented.");
	// TODO:
	return 0.f;
};

constexpr auto BounceInOut =
[](float current, float start, float change, float duration)
{
	SCION_ERROR("EASE_IN_OUT_BOUNCE, has not been implemented.");
	// TODO:
	return 0.f;
};

// Circular easing: sqrt(1-t^2)
constexpr auto CircularIn =
[](float current, float start, float change, float duration)
{
	SCION_ERROR("EASE_IN_CIRC, has not been implemented.");
	// TODO:
	return 0.f;
};

// Circular easing: sqrt(1-t^2)
constexpr auto CircularOut =
[](float current, float start, float change, float duration)
{
	SCION_ERROR("EASE_OUT_CIRC, has not been implemented.");
	// TODO:
	return 0.f;
};

// Circular easing: sqrt(1-t^2)
constexpr auto CircularInOut = 
[](float current, float start, float change, float duration)
{
	SCION_ERROR("EASE_IN_OUT_CIRC, has not been implemented.");
	// TODO:
	return 0.f;
};

std::unordered_map<EEasingFunc, EasingFunction> g_mapEasingFunctions = {
	{ EEasingFunc::LINEAR, Linear },
	{ EEasingFunc::EASE_IN_QUAD, EaseQuadIn },
	{ EEasingFunc::EASE_OUT_QUAD, EaseQuadOut },
	{ EEasingFunc::EASE_IN_OUT_QUAD, EaseQuadInOut },
	{ EEasingFunc::EASE_IN_SINE, SineIn },
	{ EEasingFunc::EASE_OUT_SINE, SineOut },
	{ EEasingFunc::EASE_IN_OUT_SINE, SineInOut },
	{ EEasingFunc::EASE_IN_ELASTIC, ElasticIn },
	{ EEasingFunc::EASE_OUT_ELASTIC, ElasticOut },
	{ EEasingFunc::EASE_IN_OUT_ELASTIC, ElasticInOut },
	{ EEasingFunc::EASE_IN_EXPONENTIAL, ExponentialIn },
	{ EEasingFunc::EASE_OUT_EXPONENTIAL, ExponentialOut },
	{ EEasingFunc::EASE_IN_OUT_EXPONENTIAL, ExponentialInOut },
	{ EEasingFunc::EASE_IN_BOUNCE, BounceIn },
	{ EEasingFunc::EASE_OUT_BOUNCE, BounceOut },
	{ EEasingFunc::EASE_IN_OUT_BOUNCE, BounceInOut },
	{ EEasingFunc::EASE_IN_CIRC, CircularIn },
	{ EEasingFunc::EASE_OUT_CIRC, CircularOut },
	{ EEasingFunc::EASE_IN_OUT_CIRC, CircularInOut }
};

// clang-format on

Tween::Tween()
	: Tween( 0.f, 1.f, 2.f, EEasingFunc::LINEAR )
{
}

Tween::Tween( float start, float finish, float totalDuration, EEasingFunc func )
	: m_eEasingFunc{ func }
	, m_TotalDuration{ totalDuration }
	, m_StartValue{ start }
	, m_CurrentValue{ start }
	, m_TimePassed{ 0.f }
	, m_TotalDistance{ finish - start }
	, m_bFinished{ false }
{
}

void Tween::Update( const float dt )
{
	if ( m_bFinished )
		return;

	m_TimePassed += dt;
	m_CurrentValue = GetEasingFunc( m_eEasingFunc, m_TimePassed, m_StartValue, m_TotalDistance, m_TotalDuration );
	if ( m_TimePassed >= m_TotalDuration )
	{
		m_CurrentValue = m_StartValue + m_TotalDistance;
		m_bFinished = true;
	}
}

float Tween::GetEasingFunc( EEasingFunc func, float currentTime, float start, float change, float duration )
{
	return g_mapEasingFunctions[ func ]( currentTime, start, change, duration );
}

} // namespace SCION_UTIL
