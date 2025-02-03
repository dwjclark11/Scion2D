#pragma once
#include "Core/ECS/Registry.h"

namespace SCION_RENDERING
{
class Camera2D;
class SpriteBatchRenderer;
} // namespace SCION_RENDERING

namespace SCION_UTIL
{
struct SpriteLayerParams;
}

namespace SCION_CORE::Systems
{
class RenderSystem
{
  private:
	std::unique_ptr<SCION_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;

  public:
	RenderSystem();
	~RenderSystem() = default;

	/*
	 * @brief Loops through all of the entities in the registry that have a sprite
	 * and transform component. Applies all the necessary transformations and adds them
	 * to a Batch to be rendered.
	 */
	void Update( SCION_CORE::ECS::Registry& registry, SCION_RENDERING::Camera2D& camera,
				 const std::vector<SCION_UTIL::SpriteLayerParams>& layerFilters = {} );

	static void CreateRenderSystemLuaBind( sol::state& lua, SCION_CORE::ECS::Registry& registry );
};
} // namespace SCION_CORE::Systems
