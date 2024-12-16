#pragma once
#include "IDisplay.h"
#include <string>

namespace SCION_EDITOR
{
class TilesetDisplay : public IDisplay
{
  private:
	std::string m_sTileset{ "" };
	int m_Selected{ -1 };

 protected:
	virtual void DrawToolbar() override;

  public:
	TilesetDisplay() = default;
	~TilesetDisplay() = default;

	virtual void Draw() override;

	inline void SetTileset( const std::string& sTileset ) { m_sTileset = sTileset; }
	inline const std::string& GetTilesetName() const { return m_sTileset; }
};
} // namespace SCION_EDITOR
