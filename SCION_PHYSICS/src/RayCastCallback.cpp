#include "Physics/RayCastCallback.h"

namespace Scion::Physics
{
RayCastCallback::RayCastCallback()
	: m_pHitFixture{ nullptr }
	, m_Point{}
	, m_Normal{}
	, m_Fraction{}
	, m_bHit{ false }
{
}

float RayCastCallback::ReportFixture( b2Fixture* pFixture, const b2Vec2& point, const b2Vec2& normal, float fraction )
{
	m_bHit = true;
	m_pHitFixture = pFixture;
	m_Point = point;
	m_Normal = normal;
	m_Fraction = fraction;

	// return m_Fraction;
	return 0.0f;
}

} // namespace Scion::Physics
