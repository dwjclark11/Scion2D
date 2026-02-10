#pragma once
#include "Core/ECS/Registry.h"
#include <sol/sol.hpp>

namespace Scion::Rendering
{
class Camera2D;
class SpriteBatchRenderer;
class TextBatchRenderer;
} // namespace Scion::Rendering

namespace Scion::Core::Systems
{
class RenderUISystem
{
  private:
	std::unique_ptr<Scion::Rendering::SpriteBatchRenderer> m_pSpriteRenderer;
	std::unique_ptr<Scion::Rendering::TextBatchRenderer> m_pTextRenderer;
	std::unique_ptr<Scion::Rendering::Camera2D> m_pCamera2D;

  public:
	RenderUISystem();
	~RenderUISystem();

	void Update( Scion::Core::ECS::Registry& registry );
	inline Scion::Rendering::Camera2D* GetCamera() { return m_pCamera2D.get(); }

	static void CreateRenderUISystemLuaBind( sol::state& lua );

};
} // namespace Scion::Core::Systems
