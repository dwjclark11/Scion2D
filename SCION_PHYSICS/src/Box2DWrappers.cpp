#include "Physics/Box2DWrappers.h"

namespace SCION_PHYSICS
{
void BodyDestroyer::operator()( b2Body* body ) const
{
	body->GetWorld()->DestroyBody( body );
}
} // namespace SCION_PHYSICS
