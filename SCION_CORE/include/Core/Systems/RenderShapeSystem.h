#pragma once

namespace Scion::Rendering
{
class Camera2D;
class ShapeRenderer;
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
	std::unique_ptr<Scion::Rendering::ShapeRenderer> m_pShapeRenderer;

  public:
	RenderShapeSystem();
	~RenderShapeSystem();

	void Update( Scion::Core::ECS::Registry& registry, Scion::Rendering::Camera2D& camera );
};
} // namespace Scion::Core::Systems
