#pragma once
#include <sol/sol.hpp>

namespace Scion::Core::ECS
{
class Registry;
}

namespace Scion::Rendering
{
class Camera2D;
class SpriteBatchRenderer;
} // namespace Scion::Rendering

namespace Scion::Core::Systems
{
class RenderSystem
{
  public:
	RenderSystem();
	~RenderSystem();

	void Update( Scion::Core::ECS::Registry& registry, Scion::Rendering::Camera2D& camera );
	static void CreateRenderSystemLuaBind( sol::state& lua, Scion::Core::ECS::Registry& registry );

  private:
	std::unique_ptr<Scion::Rendering::SpriteBatchRenderer> m_pBatchRenderer;
};
} // namespace Scion::Core::Systems
