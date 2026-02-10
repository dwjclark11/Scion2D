#include "Physics/Box2DWrappers.h"

namespace Scion::Physics
{
void BodyDestroyer::operator()( b2Body* body ) const
{
	body->GetWorld()->DestroyBody( body );
}
} // namespace Scion::Physics
