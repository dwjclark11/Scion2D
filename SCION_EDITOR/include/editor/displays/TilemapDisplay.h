#pragma once
#include "IDisplay.h"
#include <glm/glm.hpp>

namespace Scion::Rendering
{
class Camera2D;
}

namespace Scion::Core
{

struct PrefabbedEntity;

namespace ECS
{
class Entity;
}
namespace Events
{
struct KeyEvent;
}
} // namespace Scion::Core

namespace Scion::Editor
{
class TilemapDisplay : public IDisplay
{
  public:
	TilemapDisplay();
	virtual ~TilemapDisplay();

	virtual void Draw() override;
	virtual void Update() override;

  protected:
	virtual void DrawToolbar() override;

  private:
	void RenderTilemap();
	void LoadNewScene();
	void PanZoomCamera( const glm::vec2& mousePos );

	void HandleKeyPressedEvent( const Scion::Core::Events::KeyEvent& keyEvent );
	void AddPrefabbedEntityToScene( const Scion::Core::PrefabbedEntity& prefabbed );

  private:
	std::unique_ptr<Scion::Rendering::Camera2D> m_pTilemapCam;
	bool m_bWindowActive;
};
} // namespace Scion::Editor
