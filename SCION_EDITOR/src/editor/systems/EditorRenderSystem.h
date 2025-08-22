#pragma once

namespace SCION_CORE
{
namespace ECS
{
class Registry;
}
} // namespace SCION_CORE

namespace SCION_RENDERING
{
class Camera2D;
class SpriteBatchRenderer;
} // namespace SCION_RENDERING

namespace SCION_UTIL
{
struct SpriteLayerParams;
}

namespace SCION_EDITOR
{

class EditorRenderSystem
{
  public:
	EditorRenderSystem();
	~EditorRenderSystem();

	/*
	 * @brief Loops through all of the entities in the registry that have a sprite
	 * and transform component. Applies all the necessary transformations and adds them
	 * to a Batch to be rendered.
	 */
	void Update( SCION_CORE::ECS::Registry& registry, SCION_RENDERING::Camera2D& camera,
				 const std::vector<SCION_UTIL::SpriteLayerParams>& layerFilters = {} );

  private:
	std::unique_ptr<SCION_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;
};
} // namespace SCION_EDITOR

using EditorRenderSystemPtr = std::shared_ptr<SCION_EDITOR::EditorRenderSystem>;
