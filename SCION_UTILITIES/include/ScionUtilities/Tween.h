#pragma once

namespace SCION_UTIL
{

enum class EEasingFunc
{
	LINEAR,
	EASE_IN_QUAD,
	EASE_OUT_QUAD,
	EASE_IN_OUT_QUAD,
	EASE_IN_SINE,
	EASE_OUT_SINE,
	EASE_IN_OUT_SINE,
	EASE_IN_ELASTIC,
	EASE_OUT_ELASTIC,
	EASE_IN_OUT_ELASTIC,
	EASE_IN_EXPONENTIAL,
	EASE_OUT_EXPONENTIAL,
	EASE_IN_OUT_EXPONENTIAL,
	EASE_IN_BOUNCE,
	EASE_OUT_BOUNCE,
	EASE_IN_OUT_BOUNCE,
	EASE_IN_CIRC,
	EASE_OUT_CIRC,
	EASE_IN_OUT_CIRC
};

class Tween
{
  public:
	Tween();
	Tween( float start, float finish, float totalDuration, EEasingFunc func );
	~Tween() = default;

	void Update( const float dt );

	inline float TotalDistance() const { return m_TotalDistance; }
	inline float CurrentValue() const { return m_CurrentValue; }
	inline bool IsFinished() const { return m_bFinished; }

  private:
	float GetEasingFunc( EEasingFunc func, float currentTime, float start, float change, float duration );

  private:
	EEasingFunc m_eEasingFunc;
	float m_TotalDuration;
	float m_StartValue;
	float m_CurrentValue;
	float m_TimePassed;
	float m_TotalDistance;
	bool m_bFinished;
};

} // namespace SCION_UTIL
