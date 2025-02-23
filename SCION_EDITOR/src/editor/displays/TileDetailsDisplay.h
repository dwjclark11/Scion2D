#pragma once
#include "IDisplay.h"
#include <string>

namespace SCION_CORE
{
class Scene;

namespace ECS
{
struct SpriteComponent;
}
} // namespace SCION_CORE::ECS

namespace SCION_EDITOR
{

class TileDetailsDisplay : public IDisplay
{
  private:
	int m_SelectedLayer;
	std::string m_sRenameLayerBuf;
	bool m_bRename;

  private:
	void DrawSpriteComponent( SCION_CORE::ECS::SpriteComponent& sprite, SCION_CORE::Scene* pScene );

  public:
	TileDetailsDisplay();
	virtual ~TileDetailsDisplay();

	virtual void Draw() override;
};
} // namespace SCION_EDITOR
