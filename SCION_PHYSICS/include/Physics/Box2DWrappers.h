#pragma once
#include <box2d/box2d.h>
#include <memory>

namespace SCION_PHYSICS
{
using PhysicsWorld = std::shared_ptr<b2World>;

/*
 * BodyDestroyer
 * Simple custom deleter that will allow us to wrap b2Body* in a shared_ptr.
 * This will handle the deletion of the body for us.
 */
struct BodyDestroyer
{
	void operator()( b2Body* body ) const;
};

/*
 * @brief Converts a b2Body* into an std::shared_ptr with a custom deleter.
 * @param Takes in a pointer to a b2Body to convert.
 * @return Returns an std::shared_ptr of the passed in body.
 */
static std::shared_ptr<b2Body> MakeSharedBody( b2Body* body )
{
	return std::shared_ptr<b2Body>( body, BodyDestroyer{} );
}
} // namespace SCION_PHYSICS
