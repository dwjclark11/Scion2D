#pragma once
#include <box2d/box2d.h>
#include <memory>

namespace SCION_PHYSICS
{
using PhysicsWorld = std::shared_ptr<b2World>;

struct BodyDestroyer
{
	void operator()(b2Body* body) const;
};

static std::shared_ptr<b2Body> MakeSharedBody(b2Body* body)
{
	return std::shared_ptr<b2Body>(body, BodyDestroyer{});
}
} // namespace SCION_PHYSICS
