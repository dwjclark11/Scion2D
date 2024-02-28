#include "PhysicsComponent.h"
#include <Logger/Logger.h>
#include "../../CoreUtilities/CoreEngineData.h"

namespace SCION_CORE::ECS {

	PhysicsComponent::PhysicsComponent(const PhysicsAttributes& physicsAttr)
		: m_pRigidBody{ nullptr }, m_InitialAttribs{physicsAttr}
	{

	}

	void PhysicsComponent::Init(SCION_PHYSICS::PhysicsWorld pPhysicsWorld, int windowWidth, int windowHeight)
	{
		if (!pPhysicsWorld)
		{
			SCION_ERROR("Failed to initialize the physics component - Physics world is nullptr!");
			return;
		}

		bool bCircle{ m_InitialAttribs.bCircle };

		// Create the body def 
		b2BodyDef bodyDef{};
		bodyDef.type = static_cast<b2BodyType>(m_InitialAttribs.eType);
		auto PIXELS_TO_METERS = CoreEngineData::GetInstance().PixelsToMeters();

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
		m_pRigidBody = SCION_PHYSICS::MakeSharedBody(pPhysicsWorld->CreateBody(&bodyDef));

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
			circleShape.m_radius = m_InitialAttribs.radius * m_InitialAttribs.scale.x;
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

	PhysicsComponent::PhysicsComponent()
		: PhysicsComponent(PhysicsAttributes{})
	{
	}

	void PhysicsComponent::CreatePhysicsLuaBind(sol::state& lua, entt::registry& registry)
	{
		lua.new_enum<RigidBodyType>(
			"BodyType", {
				{"Static", RigidBodyType::STATIC },
				{"Kinematic", RigidBodyType::KINEMATIC},
				{"Dynamic", RigidBodyType::DYNAMIC }
			}
		);

		lua.new_usertype<PhysicsAttributes>(
			"PhysicsAttributes",
			sol::call_constructor,
			sol::factories(
				[] {
					return PhysicsAttributes{};
				}
				// TODO: Add more specific ctor
			),
			"eType", &PhysicsAttributes::eType,
			"density", &PhysicsAttributes::density,
			"friction", &PhysicsAttributes::friction,
			"restitution", &PhysicsAttributes::restitution,
			"restitutionThreshold", &PhysicsAttributes::restitutionThreshold,
			"radius", &PhysicsAttributes::radius,
			"gravityScale", &PhysicsAttributes::gravityScale,
			"position", &PhysicsAttributes::position,
			"scale", &PhysicsAttributes::scale,
			"boxSize", &PhysicsAttributes::boxSize,
			"offset", &PhysicsAttributes::offset,
			"bCircle", &PhysicsAttributes::bCircle,
			"bBoxShape", &PhysicsAttributes::bBoxShape,
			"bFixedRotation", &PhysicsAttributes::bFixedRotation
			// TODO: Add in filters and other properties as needed
		);

		auto& pPhysicsWorld = registry.ctx().get<SCION_PHYSICS::PhysicsWorld>();

		if (!pPhysicsWorld)
		{
			return;
		}

		lua.new_usertype<PhysicsComponent>(
			"PhysicsComp",
			"type_id", &entt::type_hash<PhysicsComponent>::value,
			sol::call_constructor,
			sol::factories(
				[&](const PhysicsAttributes& attr) {
					PhysicsComponent pc{ attr };
					pc.Init(pPhysicsWorld, 640, 480); // TODO: Change based on window values
					return pc;
				}
			), 
			"linear_impulse", [](PhysicsComponent& pc, const glm::vec2& impulse) {
				auto body = pc.GetBody();
				if (!body)
				{
					// TODO: Add Error
					return;
				}

				body->ApplyLinearImpulse(b2Vec2{ impulse.x, impulse.y }, body->GetPosition(), true);
			},
			"angular_impulse", [](PhysicsComponent& pc, float impulse) {
				auto body = pc.GetBody();
				if (!body)
				{
					// TODO: Add Error
					return;
				}

				body->ApplyAngularImpulse(impulse, true);
			},
			"set_linear_velocity", [](PhysicsComponent& pc, const glm::vec2& velocity) {
				auto body = pc.GetBody();
				if (!body)
				{
					// TODO: Add Error
					return;
				}

				body->SetLinearVelocity(b2Vec2{velocity.x, velocity.y});
			},
			"get_linear_velocity", [](PhysicsComponent& pc) {
				auto body = pc.GetBody();
				if (!body)
				{
					// TODO: Add Error
					return glm::vec2{ 0.f };
				}
				const auto& linearVelocity = body->GetLinearVelocity();
				return glm::vec2{linearVelocity.x, linearVelocity.y};
			},
			"set_angular_velocity", [](PhysicsComponent& pc, float angularVelocity) {
				auto body = pc.GetBody();
				if (!body)
				{
					// TODO: Add Error
					return;
				}

				body->SetAngularVelocity(angularVelocity);
			},
			"get_angular_velocity", [](PhysicsComponent& pc) {
				auto body = pc.GetBody();
				if (!body)
				{
					// TODO: Add Error
					return 0.f;
				}

				return body->GetAngularVelocity();
			},
			"set_gravity_scale", [](PhysicsComponent& pc, float gravityScale) {
				auto body = pc.GetBody();
				if (!body)
				{
					// TODO: Add Error
					return;
				}

				body->SetGravityScale(gravityScale);
			}
		);
	}

}
