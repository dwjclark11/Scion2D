#pragma once

#include <memory>

namespace SCION_RENDERING
{
class RectBatchRenderer;
class Camera2D;
} // namespace SCION_RENDERING

namespace SCION_CORE
{
class Scene;
}

namespace SCION_EDITOR
{
class GridSystem
{
  public:
	GridSystem();
	~GridSystem() = default;

	void Update( SCION_CORE::Scene& currentScene, SCION_RENDERING::Camera2D& camera );

  private:
	void UpdateIso( SCION_CORE::Scene& currentScene, SCION_RENDERING::Camera2D& camera );

  private:
	std::unique_ptr<SCION_RENDERING::RectBatchRenderer> m_pBatchRenderer;
};
} // namespace SCION_EDITOR
