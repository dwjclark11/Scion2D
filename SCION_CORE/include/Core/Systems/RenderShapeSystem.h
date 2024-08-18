#pragma once
#include "Core/ECS/Registry.h"

namespace SCION_RENDERING
{
class Camera2D;
class RectBatchRenderer;
class CircleBatchRenderer;
} // namespace SCION_RENDERING

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

	void Update( SCION_CORE::ECS::Registry& registry, SCION_RENDERING::Camera2D& camera );
};
} // namespace SCION_CORE::Systems
