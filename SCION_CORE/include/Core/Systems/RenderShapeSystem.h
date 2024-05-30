#pragma once
#include "Core/ECS/Registry.h"
#include <Rendering/Core/RectBatchRenderer.h>
#include <Rendering/Core/CircleBatchRenderer.h>

namespace SCION_CORE::Systems
{
class RenderShapeSystem
{
  private:
	SCION_CORE::ECS::Registry& m_Registry;
	std::unique_ptr<SCION_RENDERING::RectBatchRenderer> m_pRectRenderer;
	std::unique_ptr<SCION_RENDERING::CircleBatchRenderer> m_pCircleRenderer;

  public:
	RenderShapeSystem(SCION_CORE::ECS::Registry& registry);
	~RenderShapeSystem() = default;

	void Update();
};
} // namespace SCION_CORE::Systems