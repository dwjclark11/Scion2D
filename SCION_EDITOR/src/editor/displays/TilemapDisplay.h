#pragma once
#include "IDisplay.h"
#include <glm/glm.hpp>

namespace SCION_RENDERING
{
class Camera2D;
}

namespace SCION_CORE
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
} // namespace SCION_CORE

namespace SCION_EDITOR
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

	void HandleKeyPressedEvent( const SCION_CORE::Events::KeyEvent& keyEvent );
	void AddPrefabbedEntityToScene( const SCION_CORE::PrefabbedEntity& prefabbed );

  private:
	std::unique_ptr<SCION_RENDERING::Camera2D> m_pTilemapCam;
	bool m_bWindowActive;
};
} // namespace SCION_EDITOR
