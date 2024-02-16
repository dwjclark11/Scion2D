#include "PhysicsSystem.h"
#include "../ECS/Components/BoxColliderComponent.h"
#include "../ECS/Components/CircleColliderComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Components/PhysicsComponent.h"
#include <Logger/Logger.h>

using namespace SCION_CORE::ECS;

namespace SCION_CORE::Systems {

	PhysicsSystem::PhysicsSystem(SCION_CORE::ECS::Registry& registry)
		: m_Registry{registry}
	{

	}

	void PhysicsSystem::Update(entt::registry& registry)
	{
		auto boxView = registry.view<PhysicsComponent, TransformComponent, BoxColliderComponent>();
		auto hScaledWidth = 640.f * PIXELS_TO_METERS * 0.5f;
		auto hScaledHeight = 480.f * PIXELS_TO_METERS * 0.5f;

		for (auto entity : boxView)
		{
			auto& physics = boxView.get<PhysicsComponent>(entity);
			auto pRigidBody = physics.GetBody();

			if (!pRigidBody)
				continue;

			auto& transform = boxView.get<TransformComponent>(entity);
			auto& boxCollider = boxView.get<BoxColliderComponent>(entity);

			const auto& bodyPosition = pRigidBody->GetPosition();

			transform.position.x = (hScaledWidth + bodyPosition.x) * METERS_TO_PIXELS -
				(boxCollider.width * transform.scale.x) * 0.5f - boxCollider.offset.x;

			transform.position.y = (hScaledHeight + bodyPosition.y) * METERS_TO_PIXELS -
				(boxCollider.height * transform.scale.y) * 0.5f - boxCollider.offset.y;

			if (!pRigidBody->IsFixedRotation())
				transform.rotation = glm::degrees(pRigidBody->GetAngle());
		}

		auto circleView = registry.view<PhysicsComponent, TransformComponent, CircleColliderComponent>();
		for (auto entity : circleView)
		{
			auto& physics = circleView.get<PhysicsComponent>(entity);
			auto pRigidBody = physics.GetBody();

			if (!pRigidBody)
				continue;

			auto& transform = circleView.get<TransformComponent>(entity);
			auto& circleCollider = circleView.get<CircleColliderComponent>(entity);

			const auto& bodyPosition = pRigidBody->GetPosition();

			transform.position.x = (hScaledWidth + bodyPosition.x) * METERS_TO_PIXELS -
				(circleCollider.radius * transform.scale.x) - circleCollider.offset.x;

			transform.position.y = (hScaledHeight + bodyPosition.y) * METERS_TO_PIXELS -
				(circleCollider.radius * transform.scale.y) - circleCollider.offset.y;

			if (!pRigidBody->IsFixedRotation())
				transform.rotation = glm::degrees(pRigidBody->GetAngle());
		}
	}
}