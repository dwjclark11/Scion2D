#include "PhysicsSystem.h"
#include "../ECS/Components/BoxColliderComponent.h"
#include "../ECS/Components/CircleColliderComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Components/PhysicsComponent.h"
#include <Logger/Logger.h>
#include "../CoreUtilities/CoreEngineData.h"

using namespace SCION_CORE::ECS;

namespace SCION_CORE::Systems {

	PhysicsSystem::PhysicsSystem(SCION_CORE::ECS::Registry& registry)
		: m_Registry{registry}
	{

	}

	void PhysicsSystem::Update(entt::registry& registry)
	{
		auto boxView = registry.view<PhysicsComponent, TransformComponent, BoxColliderComponent>();
		auto& coreEngine = CoreEngineData::GetInstance();

		float hScaledWidth = coreEngine.ScaledWidth() * 0.5f;
		float hScaledHeight = coreEngine.ScaledHeight() * 0.5f;

		const float M2P = coreEngine.MetersToPixels();

		for (auto entity : boxView)
		{
			auto& physics = boxView.get<PhysicsComponent>(entity);
			auto pRigidBody = physics.GetBody();

			if (!pRigidBody)
				continue;

			// If the entity is static, we can skip it
			if (pRigidBody->GetType() == b2BodyType::b2_staticBody)
				continue;

			auto& transform = boxView.get<TransformComponent>(entity);
			auto& boxCollider = boxView.get<BoxColliderComponent>(entity);

			const auto& bodyPosition = pRigidBody->GetPosition();

			transform.position.x = (hScaledWidth + bodyPosition.x) * M2P -
				(boxCollider.width * transform.scale.x) * 0.5f - boxCollider.offset.x;

			transform.position.y = (hScaledHeight + bodyPosition.y) * M2P -
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

			// If the entity is static, we can skip it
			if (pRigidBody->GetType() == b2BodyType::b2_staticBody)
				continue;

			auto& transform = circleView.get<TransformComponent>(entity);
			auto& circleCollider = circleView.get<CircleColliderComponent>(entity);

			const auto& bodyPosition = pRigidBody->GetPosition();

			transform.position.x = (hScaledWidth + bodyPosition.x) * M2P -
				(circleCollider.radius * transform.scale.x) - circleCollider.offset.x;

			transform.position.y = (hScaledHeight + bodyPosition.y) * M2P -
				(circleCollider.radius * transform.scale.y) - circleCollider.offset.y;

			if (!pRigidBody->IsFixedRotation())
				transform.rotation = glm::degrees(pRigidBody->GetAngle());
		}
	}
}