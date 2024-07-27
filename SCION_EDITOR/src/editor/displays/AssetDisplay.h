#pragma once
#include "IDisplay.h"
#include <string>

namespace SCION_UTIL
{
enum class AssetType;
}

namespace SCION_EDITOR
{
class AssetDisplay : public IDisplay
{
  private:
	const std::vector<std::string> m_SelectableTypes{ "TEXTURES", "FONTS", "MUSIC", "SOUNDFX", "SCENES" };
	bool m_bAssetTypeChanged, m_bRename;
	bool m_bWindowSelected, m_bWindowHovered;
	std::string m_sSelectedAssetName, m_sSelectedType, m_sDragSource, m_sRenameBuf;
	SCION_UTIL::AssetType m_eSelectedType;
	float m_AssetSize;
	int m_SelectedID;

  private:
	void SetAssetType();
	void DrawSelectedAssets();
	unsigned int GetTextureID( const std::string& sAssetName ) const; 
	bool DoRenameAsset( const std::string& sOldName, const std::string& sNewName ) const;
	void CheckRename( const std::string& sCheckName ) const;
	void OpenAssetContext( const std::string& sAssetName );

  public:
	AssetDisplay();
	~AssetDisplay() = default;

	virtual void Draw() override;
	virtual void Update() override;
};
} // namespace SCION_EDITOR