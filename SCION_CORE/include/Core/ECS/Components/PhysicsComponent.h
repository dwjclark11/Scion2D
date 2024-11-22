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
	float density{ 1.f }, friction{ 0.2f }, restitution{ 0.2f };
	float restitutionThreshold{ 1.f }, radius{ 0.f }, gravityScale{ 1.f };

	glm::vec2 position{ 0.f }, scale{ 1.f }, boxSize{ 0.f }, offset{ 0.f };
	bool bCircle{ false }, bBoxShape{ true }, bFixedRotation{ true }, bIsSensor{ false };
	bool bIsBullet{ false };

	/*
	* Filters - This is used to filter collisions on shapes.
	* It affects shape-vs-shape collision and shape-versus-query collision (such as b2World_CastRay).
	* See https://box2d.org/documentation/group__shape.html#structb2_filter for more information.
	*/
	uint16_t filterCategory{ 0 };
	uint16_t filterMask{ 0 };
	int16_t groupIndex{ 0 };

	/* This is the user specified data for each body. */
	SCION_PHYSICS::ObjectData objectData{};
};

class PhysicsComponent
{
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
