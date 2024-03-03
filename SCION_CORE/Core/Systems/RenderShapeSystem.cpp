#include "RenderShapeSystem.h"
#include "../ECS/Components/BoxColliderComponent.h"
#include "../ECS/Components/CircleColliderComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include "../Resources/AssetManager.h"
#include "../CoreUtilities/CoreEngineData.h"

#include <Rendering/Core/Camera2D.h>
#include <Rendering/Essentials/Primitives.h>

using namespace SCION_CORE::ECS;
using namespace SCION_RENDERING;
using namespace SCION_RESOURCES;

namespace SCION_CORE::Systems {

	RenderShapeSystem::RenderShapeSystem(SCION_CORE::ECS::Registry& registry)
		: m_Registry{ registry }
		, m_pRectRenderer{ std::make_unique<RectBatchRenderer>() }
		, m_pCircleRenderer{ std::make_unique<CircleBatchRenderer>() }
	{

	}

	void RenderShapeSystem::Update()
	{
		if (!CoreEngineData::GetInstance().RenderCollidersEnabled())
			return;

		auto& camera = m_Registry.GetContext<std::shared_ptr<Camera2D>>();
		auto& assetManager = m_Registry.GetContext<std::shared_ptr<AssetManager>>();

		auto colorShader = assetManager->GetShader("color");
		auto cam_mat = camera->GetCameraMatrix();

		colorShader->Enable();
		colorShader->SetUniformMat4("uProjection", cam_mat);
		m_pRectRenderer->Begin();

		auto boxView = m_Registry.GetRegistry().view<TransformComponent, BoxColliderComponent>();
		for (auto entity : boxView)
		{
			const auto& transform = boxView.get<TransformComponent>(entity);
			const auto& boxCollider = boxView.get<BoxColliderComponent>(entity);

			glm::mat4 model{1.f};

			if (transform.rotation > 0.f || transform.rotation < 0.f ||
				transform.scale.x > 1.f || transform.scale.x < 1.f ||
				transform.scale.y > 1.f || transform.scale.y < 1.f)
			{
				model = glm::translate(model, glm::vec3{transform.position, 0.f});
				model = glm::translate(model, glm::vec3{ (boxCollider.width * transform.scale.x) * 0.5f, (boxCollider.height * transform.scale.y) * 0.5f, 0.f});

				model = glm::rotate(model, glm::radians(transform.rotation), glm::vec3{0.f, 0.f, 1.f});
				model = glm::translate(model, glm::vec3{ (boxCollider.width * transform.scale.x) * -0.5f, (boxCollider.height * transform.scale.y) * -0.5f, 0.f});

				model = glm::scale(model, glm::vec3{transform.scale, 1.f});

				model = glm::translate(model, glm::vec3{-transform.position, 0.f});
			}
			
			Rect rect{
				.position = glm::vec2{ 
					transform.position.x + boxCollider.offset.x, 
					transform.position.y + boxCollider.offset.y
				},
				.width = boxCollider.width * transform.scale.x,
				.height = boxCollider.height * transform.scale.y,
				.color = Color{255, 0, 0, 135}
			};

			m_pRectRenderer->AddRect(rect, model);
		}

		m_pRectRenderer->End();
		m_pRectRenderer->Render();
		colorShader->Disable();


		auto circleShader = assetManager->GetShader("circle");

		circleShader->Enable();
		circleShader->SetUniformMat4("uProjection", cam_mat);
		m_pCircleRenderer->Begin();

		auto circleView = m_Registry.GetRegistry().view<TransformComponent, CircleColliderComponent>();
		for (auto entity : circleView)
		{
			const auto& transform = circleView.get<TransformComponent>(entity);
			const auto& circleCollider = circleView.get<CircleColliderComponent>(entity);

			glm::vec4 circle{
				transform.position.x + circleCollider.offset.x,
				transform.position.y + circleCollider.offset.y,
				circleCollider.radius * transform.scale.x * 2,
				circleCollider.radius * transform.scale.y * 2
			};

			m_pCircleRenderer->AddCircle(circle, Color{0, 255, 0, 135}, 1.f);
		}

		m_pCircleRenderer->End();
		m_pCircleRenderer->Render();
		circleShader->Disable();

	}

}