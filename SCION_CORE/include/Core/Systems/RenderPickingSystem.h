#pragma once
#include <memory>

namespace SCION_RENDERING
{
class Camera2D;
class PickingBatchRenderer;
} // namespace SCION_RENDERING

namespace SCION_CORE::ECS
{
class Registry;
}

namespace SCION_CORE::Systems
{
class RenderPickingSystem
{
  private:
	std::unique_ptr<SCION_RENDERING::PickingBatchRenderer> m_pBatchRenderer;

  public:
	RenderPickingSystem();
	~RenderPickingSystem();

	void Update( SCION_CORE::ECS::Registry& registry, SCION_RENDERING::Camera2D& camera );
};
} // namespace SCION_CORE::Systems
