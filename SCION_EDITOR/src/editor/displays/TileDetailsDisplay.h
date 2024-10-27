#pragma once
#include "IDisplay.h"
#include <string>

namespace SCION_CORE::ECS
{
struct SpriteComponent;
}

namespace SCION_EDITOR
{

class SceneObject;

class TileDetailsDisplay : public IDisplay
{
  private:
	int m_SelectedLayer;
	std::string m_sRenameLayerBuf;
	bool m_bRename;

  private:
	void DrawSpriteComponent(SCION_CORE::ECS::SpriteComponent& sprite, SceneObject* pScene);

  public:
	TileDetailsDisplay();
	virtual ~TileDetailsDisplay();

	virtual void Draw() override;
};
} // namespace SCION_EDITOR
