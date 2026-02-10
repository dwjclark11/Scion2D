#pragma once

namespace Scion::Rendering
{
class Camera2D;
class RectBatchRenderer;
class CircleBatchRenderer;
} // namespace Scion::Rendering

namespace Scion::Core::ECS
{
class Registry;
}

namespace Scion::Core::Systems
{
class RenderShapeSystem
{
  private:
	std::unique_ptr<Scion::Rendering::RectBatchRenderer> m_pRectRenderer;
	std::unique_ptr<Scion::Rendering::CircleBatchRenderer> m_pCircleRenderer;

  public:
	RenderShapeSystem();
	~RenderShapeSystem() = default;

	void Update( Scion::Core::ECS::Registry& registry, Scion::Rendering::Camera2D& camera );
};
} // namespace Scion::Core::Systems
