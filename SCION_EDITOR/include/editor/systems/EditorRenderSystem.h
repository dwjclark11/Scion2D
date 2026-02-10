#pragma once

namespace Scion::Core
{
namespace ECS
{
class Registry;
}
} // namespace Scion::Core

namespace Scion::Rendering
{
class Camera2D;
class SpriteBatchRenderer;
} // namespace Scion::Rendering

namespace Scion::Utilities
{
struct SpriteLayerParams;
}

namespace Scion::Editor
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
	void Update( Scion::Core::ECS::Registry& registry, Scion::Rendering::Camera2D& camera,
				 const std::vector<Scion::Utilities::SpriteLayerParams>& layerFilters = {} );

  private:
	std::unique_ptr<Scion::Rendering::SpriteBatchRenderer> m_pBatchRenderer;
};
} // namespace Scion::Editor

using EditorRenderSystemPtr = std::shared_ptr<Scion::Editor::EditorRenderSystem>;
