#include "PhysicsComponent.h"
#include <Logger/Logger.h>

namespace SCION_CORE::ECS {

	PhysicsComponent::PhysicsComponent(SCION_PHYSICS::PhysicsWorld pPhysicsWorld, const PhysicsAttributes& physicsAttr)
		: m_pPhysicsWorld{ pPhysicsWorld }, m_pRigidBody{ nullptr }, m_InitialAttribs{physicsAttr}
	{

	}

	void PhysicsComponent::Init(int windowWidth, int windowHeight)
	{
		if (!m_pPhysicsWorld)
		{
			SCION_ERROR("Failed to initialize the physics component - Physics world is nullptr!");
			return;
		}

		bool bCircle{ m_InitialAttribs.bCircle };

		// Create the body def 
		b2BodyDef bodyDef{};
		bodyDef.type = static_cast<b2BodyType>(m_InitialAttribs.eType);

		// Set the initial position of the body
		bodyDef.position.Set(
			(m_InitialAttribs.position.x + m_InitialAttribs.offset.x - (windowWidth * 0.5f) + 
			((bCircle ? m_InitialAttribs.radius : m_InitialAttribs.boxSize.x) * 
				m_InitialAttribs.scale.x) * 0.5f) * PIXELS_TO_METERS,
			(m_InitialAttribs.position.y + m_InitialAttribs.offset.y - (windowHeight * 0.5f) +
				((bCircle ? m_InitialAttribs.radius : m_InitialAttribs.boxSize.y) *
					m_InitialAttribs.scale.y) * 0.5f) * PIXELS_TO_METERS
		);

		bodyDef.gravityScale = m_InitialAttribs.gravityScale;
		bodyDef.fixedRotation = m_InitialAttribs.bFixedRotation;

		// Create the Rigid Body
		m_pRigidBody = SCION_PHYSICS::MakeSharedBody(m_pPhysicsWorld->CreateBody(&bodyDef));

		if (!m_pRigidBody)
		{
			SCION_ERROR("Failed to create the rigid body");
			return;
		}

		// Create the shape
		b2PolygonShape polyShape;
		b2CircleShape circleShape;

		if (bCircle)
		{
			circleShape.m_radius = m_InitialAttribs.radius;
		}
		else if (m_InitialAttribs.bBoxShape)
		{
			polyShape.SetAsBox(
				PIXELS_TO_METERS * m_InitialAttribs.boxSize.x * m_InitialAttribs.scale.x * 0.5f,
				PIXELS_TO_METERS * m_InitialAttribs.boxSize.y * m_InitialAttribs.scale.y * 0.5f
			);
		}
		else
		{
			// TODO: Create your polygon shape
		}

		// Create the fixture def
		b2FixtureDef fixtureDef{};
		if (bCircle)
			fixtureDef.shape = &circleShape;
		else
			fixtureDef.shape = &polyShape;

		fixtureDef.density = m_InitialAttribs.density;
		fixtureDef.friction = m_InitialAttribs.friction;
		fixtureDef.restitution = m_InitialAttribs.restitution;
		fixtureDef.restitutionThreshold = m_InitialAttribs.restitutionThreshold;

		auto pFixture = m_pRigidBody->CreateFixture(&fixtureDef);
		if (!pFixture)
		{
			SCION_ERROR("Failed to create the rigid body fixture!");
		}
	}

	void PhysicsComponent::CreatePhysicsLuaBind(sol::state& lua, entt::registry& registry)
	{
		// TODO: Challenge create the beginning of the lua bindings 
		// Bind the Physics Attributes
		// Bind the component
	}
}
