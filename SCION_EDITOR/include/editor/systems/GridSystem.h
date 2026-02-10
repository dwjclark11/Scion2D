#pragma once

namespace Scion::Rendering
{
class RectBatchRenderer;
class Camera2D;
} // namespace Scion::Rendering

namespace Scion::Core
{
class Scene;
}

namespace Scion::Editor
{
class GridSystem
{
  public:
	GridSystem();
	~GridSystem() = default;

	void Update( Scion::Core::Scene& currentScene, Scion::Rendering::Camera2D& camera );

  private:
	void UpdateIso( Scion::Core::Scene& currentScene, Scion::Rendering::Camera2D& camera );

  private:
	std::unique_ptr<Scion::Rendering::RectBatchRenderer> m_pBatchRenderer;
};
} // namespace Scion::Editor
