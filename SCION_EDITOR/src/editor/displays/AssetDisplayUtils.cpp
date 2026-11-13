#include "editor/displays/AssetDisplayUtils.h"
#include "ScionUtilities/ScionUtilities.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "ScionFilesystem/Dialogs/FileDialog.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "Logger/Logger.h"

#include "Sounds/AudioPlayer/AudioPlayer.hpp"
#include "Sounds/Essentials/Audio.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

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

struct AddAssetParams
{
	std::string sAssetName{};
	std::string sFilepath{};
	Scion::Utilities::AssetType eAssetType;
	std::optional<bool> optbTileset{ std::nullopt };
	std::optional<bool> optbPixelArt{ std::nullopt };
	std::optional<bool> optbSdfFont{ std::nullopt };
	std::optional<float> optFontSize{ std::nullopt };
	std::optional<std::string> optVertPath{ std::nullopt };
	std::optional<std::string> optFragPath{ std::nullopt };
};

class AssetModalCreator
{
  public:
	AssetModalCreator() {}

	bool AddAssetBasedOnType( const AddAssetParams& assetParams )
	{
		auto& assetManager = MAIN_REGISTRY().GetAssetManager();
		switch ( assetParams.eAssetType )
		{
		case Scion::Utilities::AssetType::TEXTURE: {
			SCION_ASSERT( assetParams.optbPixelArt && assetParams.optbTileset &&
						  "These must be set when adding textures" );

			return assetManager.AddTexture(
				assetParams.sAssetName, assetParams.sFilepath, *assetParams.optbPixelArt, *assetParams.optbTileset );
		}
		case Scion::Utilities::AssetType::FONT: {
			SCION_ASSERT( assetParams.optbSdfFont && assetParams.optFontSize && "These must be set when adding fonts" );

			return assetManager.AddFont(
				assetParams.sAssetName, assetParams.sFilepath, *assetParams.optFontSize );
		}
		case Scion::Utilities::AssetType::SOUNDFX: {
			return assetManager.AddAudio(
				assetParams.sAssetName, assetParams.sFilepath, Scion::Sounds::AudioType::Soundfx );
		}
		case Scion::Utilities::AssetType::MUSIC: {
			return assetManager.AddAudio(
				assetParams.sAssetName, assetParams.sFilepath, Scion::Sounds::AudioType::Music );
		}
		case Scion::Utilities::AssetType::SCENE: return false;
		case Scion::Utilities::AssetType::SHADER: {
			SCION_ASSERT( assetParams.optFragPath && assetParams.optVertPath &&
						  "Vert and Frag paths must be set for shaders." );
			return assetManager.AddShader( assetParams.sAssetName, *assetParams.optVertPath, *assetParams.optFragPath );
		}
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

		if ( ImGui::BeginPopupModal( "Add New Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize ) )
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

		if ( ImGui::BeginPopupModal( sAssetType.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize ) )
		{
			if ( m_AddAssetParams.eAssetType != eAssetType )
			{
				m_AddAssetParams.eAssetType = eAssetType;
			}

			std::string sCheckName{ m_AddAssetParams.sAssetName.data() };
			std::string sCheckFilepath{ m_AddAssetParams.sFilepath.data() };

			ImGui::InputText( "##assetName", &m_AddAssetParams.sAssetName );
			std::string sNameError{ CheckForAsset( sCheckName, eAssetType ) };

			if ( eAssetType != Scion::Utilities::AssetType::SHADER )
			{
				ImGui::InputText( "##filepath", &m_AddAssetParams.sFilepath );

				ImGui::SameLine();
				if ( ImGui::Button( "Browse" ) )
				{
					FileDialog fd{};
					m_AddAssetParams.sFilepath = fd.OpenFileDialog(
						"Open", "", Scion::Editor::AssetDisplayUtils::GetAssetFileFilters( eAssetType ) );

					if ( !m_AddAssetParams.sFilepath.empty() )
					{
						fs::path path{ m_AddAssetParams.sFilepath };
						m_AddAssetParams.sAssetName = path.stem().string();
					}
				}
			}

			if ( eAssetType == Scion::Utilities::AssetType::TEXTURE )
			{
				if ( !m_AddAssetParams.optbPixelArt )
				{
					m_AddAssetParams.optbPixelArt = false;
				}

				if ( !m_AddAssetParams.optbTileset )
				{
					m_AddAssetParams.optbTileset = false;
				}

				ImGui::Checkbox( "Pixel Art", &( *m_AddAssetParams.optbPixelArt ) );
				ImGui::Checkbox( "Tileset", &( *m_AddAssetParams.optbTileset ) );
			}
			else if ( eAssetType == Scion::Utilities::AssetType::FONT )
			{
				if ( !m_AddAssetParams.optFontSize )
				{
					m_AddAssetParams.optFontSize = 32.f;
				}

				if ( !m_AddAssetParams.optbSdfFont )
				{
					m_AddAssetParams.optbSdfFont = false;
				}

				ImGui::InlineLabel( "Font Size" );
				if ( ImGui::InputFloat( "##Font_Size", &( *m_AddAssetParams.optFontSize ), 1.f, 1.f, "%.1f" ) )
				{
					if ( *m_AddAssetParams.optFontSize < 32.f )
					{
						m_AddAssetParams.optFontSize = 32.f;
					}
				}

				ImGui::InlineLabel( "SDF Font" );
				ImGui::Checkbox( "##SDF_Font", &( *m_AddAssetParams.optbSdfFont ) );
			}
			else if ( eAssetType == Scion::Utilities::AssetType::SHADER )
			{
				if ( !m_AddAssetParams.optVertPath || !m_AddAssetParams.optFragPath )
				{
					m_AddAssetParams.optVertPath = "";
					m_AddAssetParams.optFragPath = "";
				}

				ImGui::InlineLabel( "Vert Path" );
				ImGui::InputText( "##vertpath", &( *m_AddAssetParams.optVertPath ) );
				ImGui::SameLine();
				if ( ImGui::Button( "...##vertBrowse" ) )
				{
					FileDialog fd{};
					m_AddAssetParams.optVertPath = fd.OpenFileDialog(
						"Open", "", Scion::Editor::AssetDisplayUtils::GetAssetFileFilters( eAssetType ) );
				}

				ImGui::InlineLabel( "Frag Path" );
				ImGui::InputText( "##fragpath", &( *m_AddAssetParams.optFragPath ) );
				if ( ImGui::Button( "...##fragBrowse" ) )
				{
					FileDialog fd{};
					m_AddAssetParams.optFragPath = fd.OpenFileDialog(
						"Open", "", Scion::Editor::AssetDisplayUtils::GetAssetFileFilters( eAssetType ) );
				}
			}

			if ( sNameError.empty() )
			{
				if ( ImGui::Button( "Ok" ) )
				{
					if ( eAssetType != Scion::Utilities::AssetType::SHADER )
					{
						if ( fs::exists( fs::path{ sCheckFilepath } ) )
						{
							if ( !AddAssetBasedOnType( m_AddAssetParams ) )
							{
								SCION_ERROR( "Failed to add new texture!" );
							}

							m_AddAssetParams = {};
							*pbOpen = false;
							ImGui::CloseCurrentPopup();
						}
						else
						{
							// TODO: Add filepath error!
						}
					}
					else
					{
						if ( fs::exists( fs::path{ *m_AddAssetParams.optVertPath } ) &&
							 fs::exists( fs::path{ *m_AddAssetParams.optFragPath } ) )
						{
							if ( !AddAssetBasedOnType( m_AddAssetParams ) )
							{
								SCION_ERROR( "Failed to add new texture!" );
							}

							m_AddAssetParams = {};
							*pbOpen = false;
							ImGui::CloseCurrentPopup();
						}
						else
						{
							// TODO: Add filepath error!
						}
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
				m_AddAssetParams = {};
				*pbOpen = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

  private:
	static AddAssetParams m_AddAssetParams;
};

AddAssetParams AssetModalCreator::m_AddAssetParams = AddAssetParams{};

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
