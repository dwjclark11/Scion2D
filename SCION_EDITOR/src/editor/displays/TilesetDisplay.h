#pragma once
#include "IDisplay.h"

namespace SCION_EDITOR
{
class TilesetDisplay : public IDisplay
{

  public:
	TilesetDisplay() = default;
	~TilesetDisplay() = default;

	virtual void Draw() override;

	inline void SetTileset( const std::string& sTileset ) { m_sTileset = sTileset; }
	inline const std::string& GetTilesetName() const { return m_sTileset; }

  protected:
	virtual void DrawToolbar() override;

  private:
	std::string m_sTileset{};

	struct TableSelection
	{
		int startRow{ -1 };
		int startCol{ -1 };
		int endRow{ -1 };
		int endCol{ -1 };
		bool bSelecting{ false };

		bool IsValid() const { return startRow != -1 && startCol != -1; }
		void Reset()
		{
			startRow = startCol = endRow = endCol = -1;
			bSelecting = false;
		}
	};

	TableSelection m_TableSelection;
};
} // namespace SCION_EDITOR
