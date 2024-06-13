#include "Physics/Box2DWrappers.h"

void SCION_PHYSICS::BodyDestroyer::operator()( b2Body* body ) const
{
	body->GetWorld()->DestroyBody( body );
}
