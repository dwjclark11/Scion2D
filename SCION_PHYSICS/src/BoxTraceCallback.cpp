#include "Physics/BoxTraceCallback.h"

namespace Scion::Physics
{
bool BoxTraceCallback::ReportFixture( b2Fixture* pFixture )
{
	m_Bodies.push_back( pFixture->GetBody() );

	return true;
}
} // namespace Scion::Physics
