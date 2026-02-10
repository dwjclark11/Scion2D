#pragma once

namespace Scion::Rendering
{
class Camera2D;
class PickingBatchRenderer;
} // namespace Scion::Rendering

namespace Scion::Core::ECS
{
class Registry;
}

namespace Scion::Core::Systems
{
class RenderPickingSystem
{
  private:
	std::unique_ptr<Scion::Rendering::PickingBatchRenderer> m_pBatchRenderer;

  public:
	RenderPickingSystem();
	~RenderPickingSystem();

	void Update( Scion::Core::ECS::Registry& registry, Scion::Rendering::Camera2D& camera );
};
} // namespace Scion::Core::Systems
