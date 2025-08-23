#pragma once
#include "IDisplay.h"

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
  public:
	TileDetailsDisplay();
	virtual ~TileDetailsDisplay();

	virtual void Draw() override;

  private:
	void DrawSpriteComponent( SCION_CORE::ECS::SpriteComponent& sprite, SCION_CORE::Scene* pScene );

  private:
	int m_SelectedLayer;
	int m_DeleteLayer;
	std::string m_sRenameLayerBuf;
	bool m_bRename;
	bool m_bDeleteLayer;
};
} // namespace SCION_EDITOR
