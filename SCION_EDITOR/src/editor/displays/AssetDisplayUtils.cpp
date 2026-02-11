#include "editor/displays/AssetDisplayUtils.h"
#include "ScionUtilities/ScionUtilities.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "ScionFilesystem/Dialogs/FileDialog.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "Logger/Logger.h"
#include <imgui.h>

namespace fs = std::filesystem;

#define IMAGE_FILTERS std::vector<const char*>{ "*.png", "*.bmp", "*.jpg" }
#define FONT_FILTERS                                                                                                   \
	std::vector<const char*>{                                                                                          \
		"*.ttf" /* add more font types */                                                                              \
	}
#define MUSIC_FILTERS std::vector<const char*>{ "*.mp3", "*.wav", "*.ogg" }

#define SOUNDFX_FILTERS std::vector<const char*>{ "*.mp3", "*.wav", "*.ogg" }

constexpr const char* IMAGE_DESC = "Image Files (*.png, *.bmp, *.jpg)";
constexpr const char* FONT_DESC = "Fonts (*.ttf)";
constexpr const char* MUSIC_DESC = "Music Files (*.mp3, *.wav, *.ogg)";
constexpr const char* SOUNDFX_DESC = "Soundfx Files (*.mp3, *.wav, *.ogg)";

using namespace Scion::Filesystem;
using namespace Scion::Editor;

static const std::map<std::string, Scion::Core::EMapType> g_mapStringToMapTypes{
	{ "Grid", Scion::Core::EMapType::Grid }, { "IsoGrid", Scion::Core::EMapType::IsoGrid } };

namespace
{
class AssetModalCreator
{
  public:
	AssetModalCreator() {}

	bool AddAssetBasedOnType( const std::string& sAssetName, const std::string& sFilepath,
							  Scion::Utilities::AssetType eAssetType, bool bPixelArt = true, bool bTileset = false,
							  float fontSize = 32.f )
	{
		auto& assetManager = MAIN_REGISTRY().GetAssetManager();
		switch ( eAssetType )
		{
		case Scion::Utilities::AssetType::TEXTURE:
			return assetManager.AddTexture( sAssetName, sFilepath, bPixelArt, bTileset );
		case Scion::Utilities::AssetType::FONT: return assetManager.AddFont( sAssetName, sFilepath, fontSize );
		case Scion::Utilities::AssetType::SOUNDFX: return assetManager.AddSoundFx( sAssetName, sFilepath );
		case Scion::Utilities::AssetType::MUSIC: return assetManager.AddMusic( sAssetName, sFilepath );
		case Scion::Utilities::AssetType::SCENE: return false;
		}
		return false;
	}

	std::string CheckForAsset( const std::string& sAssetName, Scion::Utilities::AssetType eAssetType )
	{
		std::string sError{};
		if ( sAssetName.empty() )
		{
			sError = "Asset name cannot be empty!";
		}
		else if ( eAssetType == Scion::Utilities::AssetType::SCENE )
		{
			if ( SCENE_MANAGER().HasScene( sAssetName ) )
				sError = fmt::format( "Scene [{}] already exists!", sAssetName );
		}
		else
		{
			if ( MAIN_REGISTRY().GetAssetManager().CheckHasAsset( sAssetName, eAssetType ) )
				sError = fmt::format( "Asset [{}] already exists!", sAssetName );
		}

		return sError;
	}

	void AddSceneModal( bool* pbOpen )
	{
		if ( *pbOpen )
			ImGui::OpenPopup( "Add New Scene" );

		if ( ImGui::BeginPopupModal( "Add New Scene" ) )
		{
			ImGui::InlineLabel( "Name" );
			static std::string sAssetName{};
			ImGui::InputText( "##assetName", sAssetName.data(), 255 );

			static std::vector<std::string> mapTypes{ "Grid", "IsoGrid" };
			static std::string sMapType{ "Grid" };
			static Scion::Core::EMapType eSelectedType{ Scion::Core::EMapType::Grid };

			ImGui::InlineLabel( "Map Type" );
			if ( ImGui::BeginCombo( "##Map Type", sMapType.c_str() ) )
			{
				for ( const auto& [ sMapStr, eMapType ] : g_mapStringToMapTypes )
				{
					if ( ImGui::Selectable( sMapStr.c_str(), sMapStr == sMapType ) )
					{
						sMapType = sMapStr;
						eSelectedType = eMapType;
					}

					ImGui::ItemToolTip( "{}",
										eMapType == Scion::Core::EMapType::IsoGrid
											? "Warning! IsoGrid maps are not fully supported."
											: "2D Grid tile map." );
				}

				ImGui::EndCombo();
			}

			std::string sCheckName{ sAssetName.data() };
			std::string sNameError{ CheckForAsset( sCheckName, Scion::Utilities::AssetType::SCENE ) };

			if ( sNameError.empty() )
			{
				if ( ImGui::Button( "Ok" ) )
				{
					if ( !SCENE_MANAGER().AddScene( sCheckName, eSelectedType ) )
					{
						SCION_ERROR( "Failed to add new scene [{}]", sCheckName );
					}

					sAssetName.clear();

					*pbOpen = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
			}
			else
			{
				ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f }, sNameError.c_str() );
			}

			if ( eSelectedType == Scion::Core::EMapType::IsoGrid )
			{
				ImGui::TextColored( ImVec4{ 1.f, 1.f, 0.f, 1.f }, "IsoGrid maps are not fully supported yet!" );
			}

			// We always want to be able to cancel
			if ( ImGui::Button( "Cancel" ) )
			{
				sAssetName.clear();
				*pbOpen = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	void AddAssetModal( Scion::Utilities::AssetType eAssetType, bool* pbOpen )
	{
		std::string sAssetType{ Scion::Editor::AssetDisplayUtils::AddAssetBasedOnType( eAssetType ) };

		if ( *pbOpen )
			ImGui::OpenPopup( sAssetType.c_str() );

		if ( ImGui::BeginPopupModal( sAssetType.c_str() ) )
		{
			static std::string sAssetName{};
			static std::string sFilepath{};
			static bool bTileset{ false };
			static bool bPixelArt{ false };
			static float fontSize{ 32.f };

			std::string sCheckName{ sAssetName.data() };
			std::string sCheckFilepath{ sFilepath.data() };

			ImGui::InputText( "##assetName", sAssetName.data(), 255 );
			std::string sNameError{ CheckForAsset( sCheckName, eAssetType ) };
			ImGui::InputText( "##filepath", sFilepath.data(), 255 );
			ImGui::SameLine();
			if ( ImGui::Button( "Browse" ) )
			{
				FileDialog fd{};
				sFilepath =
					fd.OpenFileDialog(
						"Open", BASE_PATH, 
						Scion::Editor::AssetDisplayUtils::GetAssetFileFilters( eAssetType ),
						Scion::Editor::AssetDisplayUtils::GetAssetDescriptionByType( eAssetType )
					);

				if ( !sFilepath.empty() )
				{
					fs::path path{ sFilepath };
					sAssetName = path.stem().string();
				}
			}

			if ( eAssetType == Scion::Utilities::AssetType::TEXTURE )
			{
				ImGui::Checkbox( "Pixel Art", &bPixelArt );
				ImGui::Checkbox( "Tileset", &bTileset );
			}
			else if ( eAssetType == Scion::Utilities::AssetType::FONT )
			{
				ImGui::InputFloat( "Font Size", &fontSize, 1.f, 1.f, "%.1f" );
			}

			if ( sNameError.empty() )
			{
				if ( ImGui::Button( "Ok" ) )
				{
					if ( fs::exists( fs::path{ sCheckFilepath } ) )
					{
						if ( !AddAssetBasedOnType(
								 sCheckName, sCheckFilepath, eAssetType, bPixelArt, bTileset, fontSize ) )
						{
							SCION_ERROR( "Failed to add new texture!" );
						}

						bTileset = false;
						bPixelArt = false;
						sAssetName.clear();
						sFilepath.clear();
						*pbOpen = false;
						ImGui::CloseCurrentPopup();
					}
					else
					{
						// TODO: Add filepath error!
					}
				}
				ImGui::SameLine();
			}
			else
			{
				ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f }, sNameError.c_str() );
			}

			// We always want to be able to cancel
			if ( ImGui::Button( "Cancel" ) )
			{
				bTileset = false;
				bPixelArt = false;
				sAssetName.clear();
				sFilepath.clear();
				*pbOpen = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
};

} // namespace

namespace Scion::Editor
{
std::vector<const char*> AssetDisplayUtils::GetAssetFileFilters( Scion::Utilities::AssetType eAssetType )
{
	switch ( eAssetType )
	{
	case Scion::Utilities::AssetType::TEXTURE: return IMAGE_FILTERS;
	case Scion::Utilities::AssetType::FONT: return FONT_FILTERS;
	case Scion::Utilities::AssetType::SOUNDFX: return SOUNDFX_FILTERS;
	case Scion::Utilities::AssetType::MUSIC: return MUSIC_FILTERS;
	}

	return {};
}

const char* AssetDisplayUtils::GetAssetDescriptionByType( Scion::Utilities::AssetType eAssetType )
{
	switch ( eAssetType )
	{
	case Scion::Utilities::AssetType::TEXTURE: return IMAGE_DESC;
	case Scion::Utilities::AssetType::FONT: return FONT_DESC;
	case Scion::Utilities::AssetType::SOUNDFX: return SOUNDFX_DESC;
	case Scion::Utilities::AssetType::MUSIC: return MUSIC_DESC;
	}

	return "Files";
}

std::string AssetDisplayUtils::AddAssetBasedOnType( Scion::Utilities::AssetType eAssetType )
{
	switch ( eAssetType )
	{
	case Scion::Utilities::AssetType::TEXTURE: return "Add Texture";
	case Scion::Utilities::AssetType::FONT: return "Add Font";
	case Scion::Utilities::AssetType::SOUNDFX: return "Add SoundFx";
	case Scion::Utilities::AssetType::MUSIC: return "Add Music";
	case Scion::Utilities::AssetType::SCENE: return "Add Scene";
	default: SCION_ASSERT( false && "Type has not been implemented!" ); return {};
	}
}

void AssetDisplayUtils::OpenAddAssetModalBasedOnType( Scion::Utilities::AssetType eAssetType, bool* pbOpen )
{
	SCION_ASSERT( eAssetType != Scion::Utilities::AssetType::NO_TYPE && "The asset type must be set!" );
	static AssetModalCreator md{};
	if ( eAssetType == Scion::Utilities::AssetType::SCENE )
	{
		md.AddSceneModal( pbOpen );
	}
	else
	{
		md.AddAssetModal( eAssetType, pbOpen );
	}
}
} // namespace Scion::Editor
