#pragma once
#include "IDisplay.h"

namespace SCION_UTIL
{
enum class AssetType;
}

namespace SCION_EDITOR
{
class AssetDisplay : public IDisplay
{
  public:
	AssetDisplay();
	~AssetDisplay() = default;

	virtual void Draw() override;
	virtual void Update() override;

  protected:
	virtual void DrawToolbar() override;

  private:
	void SetAssetType();
	void DrawSelectedAssets();
	unsigned int GetTextureID( const std::string& sAssetName ) const;
	bool DoRenameAsset( const std::string& sOldName, const std::string& sNewName ) const;
	void CheckRename( const std::string& sCheckName ) const;
	void OpenAssetContext( const std::string& sAssetName );
	void DrawSoundContext( const std::string& sAssetName );

  private:
	const std::vector<std::string> m_SelectableTypes{ "TEXTURES", "FONTS", "MUSIC", "SOUNDFX", "SCENES", "PREFABS" };
	bool m_bAssetTypeChanged;
	bool m_bRename;
	bool m_bWindowSelected;
	bool m_bWindowHovered;
	bool m_bOpenAddAssetModal;
	std::string m_sSelectedAssetName;
	std::string m_sSelectedType;
	std::string m_sDragSource;
	std::string m_sRenameBuf;
	SCION_UTIL::AssetType m_eSelectedType;
	float m_AssetSize;
	int m_SelectedID;
};
} // namespace SCION_EDITOR
