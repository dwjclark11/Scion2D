#pragma once
#include <Physics/Box2DWrappers.h>
#include <Physics/UserData.h>
#include <Physics/PhysicsUtilities.h>
#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace SCION_CORE::ECS
{

struct PhysicsAttributes
{
	SCION_PHYSICS::RigidBodyType eType{ SCION_PHYSICS::RigidBodyType::STATIC };
	/* The density of the body, usually in kg/m^2. */
	float density{ 1.f };
	/* The Coulomb (dry) friction coefficient, usually in the range [0,1]. */
	float friction{ 0.2f };
	/* The restitution (bounce) usually in the range [0,1]. */
	float restitution{ 0.2f };
	/* Restitution velocity threshold, usually in m/s.
	Collisions above this speed have restitution applied (will bounce).*/
	float restitutionThreshold{ 1.f };
	/* If a circle, the radius of the circle shape. */
	float radius{ 0.f };
	/* Scale the gravity applied to this body. Non-dimensional. */
	float gravityScale{ 1.f };
	/* The initial world position of the body. */
	glm::vec2 position{ 0.f };
	/* The amount to scale the body based on it's shapes size. */
	glm::vec2 scale{ 1.f };
	/* If a box, the width and height of the box shape. */
	glm::vec2 boxSize{ 0.f };
	/* X/Y offset of the collider based on the TL position of the owner. */
	glm::vec2 offset{ 0.f };
	/* If the entity is a circle, the circle collider is used in construction for the size. */
	bool bCircle{ false };
	/* If the entity is a box, the box collider is used in construction for the size. */
	bool bBoxShape{ true };
	/* Should the body be prevented from rotating? */
	bool bFixedRotation{ true };
	/* A sensor shape generates overlap events but never generates a collision response. */
	bool bIsSensor{ false };
	/* Treat this body as high speed object that performs continuous collision detection against dynamic and kinematic bodies,
	but not other bullet bodies.*/
	bool bIsBullet{ false };

	/*
	 * Filters - This is used to filter collisions on shapes.
	 * It affects shape-vs-shape collision and shape-versus-query collision (such as b2World_CastRay).
	 * See https://box2d.org/documentation/group__shape.html#structb2_filter for more information.
	 */
	uint16_t filterCategory{ 0 };
	/* The collision mask bits. This states the categories that this shape would accept for collision. */
	uint16_t filterMask{ 0 };
	/* Collision groups allow a certain group of objects to never collide (negative) or always collide (positive).
	A group index of zero has no effect. Non-zero group filtering always wins against the mask bits. */
	int16_t groupIndex{ 0 };
	/* This is the user specified data for each body. */
	SCION_PHYSICS::ObjectData objectData{};
};

class PhysicsComponent
{
  private:
	std::shared_ptr<b2Body> m_pRigidBody;
	std::shared_ptr<SCION_PHYSICS::UserData> m_pUserData;
	PhysicsAttributes m_InitialAttribs;

  public:
	PhysicsComponent();
	PhysicsComponent( const PhysicsAttributes& physicsAttr );
	~PhysicsComponent() = default;

	void Init( SCION_PHYSICS::PhysicsWorld pPhysicsWorld, int windowWidth, int windowHeight );
	b2Body* GetBody() { return m_pRigidBody.get(); }
	SCION_PHYSICS::UserData* GetUserData() { return m_pUserData.get(); }
	const bool IsSensor() const;

	/* The attributes may have changed. we need to make a function that will refill the attributes */
	const PhysicsAttributes& GetAttributes() const { return m_InitialAttribs; }
	PhysicsAttributes& GetChangableAttributes() { return m_InitialAttribs; }

	static void CreatePhysicsLuaBind( sol::state& lua, entt::registry& registry );
};
} // namespace SCION_CORE::ECS
