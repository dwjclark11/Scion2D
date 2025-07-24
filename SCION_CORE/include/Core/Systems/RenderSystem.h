#pragma once
#include <sol/sol.hpp>

namespace SCION_CORE::ECS
{
class Registry;
}

namespace SCION_RENDERING
{
class Camera2D;
class SpriteBatchRenderer;
} // namespace SCION_RENDERING

namespace SCION_CORE::Systems
{
class RenderSystem
{
  public:
	RenderSystem();
	~RenderSystem();

	void Update( SCION_CORE::ECS::Registry& registry, SCION_RENDERING::Camera2D& camera );
	static void CreateRenderSystemLuaBind( sol::state& lua, SCION_CORE::ECS::Registry& registry );

  private:
	std::unique_ptr<SCION_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;
};
} // namespace SCION_CORE::Systems
