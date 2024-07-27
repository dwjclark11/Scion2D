#pragma once
#include "Core/ECS/Registry.h"
#include <Rendering/Core/RectBatchRenderer.h>
#include <Rendering/Core/CircleBatchRenderer.h>

namespace SCION_CORE::Systems
{
class RenderShapeSystem
{
  private:
	std::unique_ptr<SCION_RENDERING::RectBatchRenderer> m_pRectRenderer;
	std::unique_ptr<SCION_RENDERING::CircleBatchRenderer> m_pCircleRenderer;

  public:
	RenderShapeSystem();
	~RenderShapeSystem() = default;

	void Update( SCION_CORE::ECS::Registry& registry );
};
} // namespace SCION_CORE::Systems