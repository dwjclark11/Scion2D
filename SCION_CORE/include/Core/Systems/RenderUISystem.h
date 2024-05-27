#pragma once
#include <Rendering/Core/BatchRenderer.h>
#include <Rendering/Core/TextBatchRenderer.h>
#include <Rendering/Core/Camera2D.h>
#include "Core/ECS/Registry.h"

namespace SCION_CORE::Systems {
	class RenderUISystem
	{
	private:
		SCION_CORE::ECS::Registry& m_Registry;
		std::unique_ptr<SCION_RENDERING::SpriteBatchRenderer> m_pSpriteRenderer;
		std::unique_ptr<SCION_RENDERING::TextBatchRenderer> m_pTextRenderer;
		std::unique_ptr<SCION_RENDERING::Camera2D> m_pCamera2D;

	public:
		RenderUISystem(SCION_CORE::ECS::Registry& registry);
		~RenderUISystem() = default;

		void Update(entt::registry& registry);
	};
}