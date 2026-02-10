#pragma once
#include "IDisplay.h"

namespace Scion::Core
{
class Scene;

namespace ECS
{
struct SpriteComponent;
}
} // namespace Scion::Core::ECS

namespace Scion::Editor
{

class TileDetailsDisplay : public IDisplay
{
  public:
	TileDetailsDisplay();
	virtual ~TileDetailsDisplay();

	virtual void Draw() override;

  private:
	void DrawSpriteComponent( Scion::Core::ECS::SpriteComponent& sprite, Scion::Core::Scene* pScene );

  private:
	int m_SelectedLayer;
	int m_DeleteLayer;
	std::string m_sRenameLayerBuf;
	bool m_bRename;
	bool m_bDeleteLayer;
};
} // namespace Scion::Editor
