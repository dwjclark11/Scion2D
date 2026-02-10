#include "editor/displays/AssetDisplay.h"
#include "editor/displays/AssetDisplayUtils.h"
#include "ScionUtilities/ScionUtilities.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Scripting/InputManager.h"
#include "Core/Resources/AssetManager.h"
#include "Logger/Logger.h"

#include "editor/utilities/EditorUtilities.h"
#include "editor/utilities/EditorState.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/scene/SceneManager.h"
#include "editor/loaders/ProjectLoader.h"

#include "Core/CoreUtilities/Prefab.h"
#include "Core/CoreUtilities/ProjectInfo.h"

#include <Rendering/Essentials/Shader.h>
#include <Rendering/Essentials/Texture.h>
#include <Rendering/Essentials/Font.h>
#include <Sounds/Essentials/Music.h>
#include <Sounds/Essentials/SoundFX.h>
#include <Sounds/MusicPlayer/MusicPlayer.h>
#include <Sounds/SoundPlayer/SoundFxPlayer.h>
#include <ScionFilesystem/Process/FileProcessor.h>
#include <ScionFilesystem/Utilities/FilesystemUtilities.h>

#include <imgui.h>

constexpr float DEFAULT_ASSET_SIZE = 64.f;
constexpr ImVec2 DRAG_ASSET_SIZE = ImVec2{ 32.f, 32.f };

namespace Scion::Editor
{
void AssetDisplay::SetAssetType()
{
	if ( !m_bAssetTypeChanged )
		return;

	if ( m_sSelectedType == "TEXTURES" )
	{
		m_eSelectedType = Scion::Utilities::AssetType::TEXTURE;
		m_sDragSource = std::string{ DROP_TEXTURE_SRC };
	}
	else if ( m_sSelectedType == "FONTS" )
	{
		m_eSelectedType = Scion::Utilities::AssetType::FONT;
		m_sDragSource = std::string{ DROP_FONT_SRC };
	}
	else if ( m_sSelectedType == "SOUNDFX" )
	{
		m_eSelectedType = Scion::Utilities::AssetType::SOUNDFX;
		m_sDragSource = std::string{ DROP_SOUNDFX_SRC };
	}
	else if ( m_sSelectedType == "MUSIC" )
	{
		m_eSelectedType = Scion::Utilities::AssetType::MUSIC;
		m_sDragSource = std::string{ DROP_MUSIC_SRC };
	}
	else if ( m_sSelectedType == "SCENES" )
	{
		m_eSelectedType = Scion::Utilities::AssetType::SCENE;
		m_sDragSource = std::string{ DROP_SCENE_SRC };
	}
	else if ( m_sSelectedType == "PREFABS" )
	{
		m_eSelectedType = Scion::Utilities::AssetType::PREFAB;
		m_sDragSource = std::string{ DROP_PREFAB_SRC };
	}
	else
	{
		m_eSelectedType = Scion::Utilities::AssetType::NO_TYPE;
		m_sDragSource = "NO_ASSET_TYPE";
	}

	m_bAssetTypeChanged = false;
}

unsigned int AssetDisplay::GetTextureID( const std::string& sAssetName ) const
{
	auto& assetManager = MAIN_REGISTRY().GetAssetManager();
	switch ( m_eSelectedType )
	{
	case Scion::Utilities::AssetType::TEXTURE: {
		auto pTexture = assetManager.GetTexture( sAssetName );
		if ( pTexture )
			return pTexture->GetID();

		break;
	}
	case Scion::Utilities::AssetType::FONT: {
		auto pFont = assetManager.GetFont( sAssetName );
		if ( pFont )
			return pFont->GetFontAtlasID();

		break;
	}
	case Scion::Utilities::AssetType::SOUNDFX:
	case Scion::Utilities::AssetType::MUSIC: {
		auto pTexture = assetManager.GetTexture( "music_icon" );
		if ( pTexture )
			return pTexture->GetID();

		break;
	}
	case Scion::Utilities::AssetType::SCENE: {
		auto pTexture = assetManager.GetTexture( "scene_icon" );
		if ( pTexture )
			return pTexture->GetID();

		break;
	}
	case Scion::Utilities::AssetType::PREFAB: {
		if ( auto pPrefab = assetManager.GetPrefab( sAssetName ) )
		{
			if ( auto& sprite = pPrefab->GetPrefabbedEntity().sprite )
			{
				if ( auto pTexture = assetManager.GetTexture( sprite->sTextureName ) )
				{
					return pTexture->GetID();
				}
			}
		}

		break;
	}
	}

	return 0;
}

bool AssetDisplay::DoRenameAsset( const std::string& sOldName, const std::string& sNewName ) const
{
	if ( sNewName.empty() )
		return false;

	if ( m_eSelectedType == Scion::Utilities::AssetType::SCENE )
	{
		return SCENE_MANAGER().ChangeSceneName( sOldName, sNewName );
	}
	else
	{
		return ASSET_MANAGER().ChangeAssetName( sOldName, sNewName, m_eSelectedType );
	}

	SCION_ASSERT( false && "How did it get here??" );
	return false;
}

void AssetDisplay::CheckRename( const std::string& sCheckName ) const
{
	if ( sCheckName.empty() )
	{
		ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f }, "Rename text cannot be blank!" );
		return;
	}

	bool bHasAsset{ false };

	if ( m_eSelectedType == Scion::Utilities::AssetType::SCENE )
	{
		if ( SCENE_MANAGER().CheckHasScene( sCheckName ) )
			bHasAsset = true;
	}
	else
	{
		auto& assetManager = MAIN_REGISTRY().GetAssetManager();
		if ( assetManager.CheckHasAsset( sCheckName, m_eSelectedType ) )
			bHasAsset = true;
	}

	if ( bHasAsset )
		ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f },
							fmt::format( "Asset name [{}] already exists!", sCheckName ).c_str() );
}

void AssetDisplay::OpenAssetContext( const std::string& sAssetName )
{
	DrawSoundContext( sAssetName );
	
	ImGui::SeparatorText( "Edit" );
	if ( ImGui::Selectable( ICON_FA_PEN " Rename" ) )
	{
		m_bRename = true;
	}

	if ( ImGui::Selectable( ICON_FA_TRASH " Delete" ) )
	{
		bool bSuccess{ false };
		if ( m_eSelectedType == Scion::Utilities::AssetType::SCENE )
		{
			if ( !SCENE_MANAGER().DeleteScene( sAssetName ) )
			{
				SCION_ERROR( "Failed to delete scene [{}]", sAssetName );
			}

			bSuccess = true;
		}
		else
		{
			if ( !ASSET_MANAGER().DeleteAsset( sAssetName, m_eSelectedType ) )
			{
				SCION_ERROR( "Failed to delete asset [{}].", sAssetName );
			}

			bSuccess = true;
		}

		// Whenever an asset is deleted, we want to save the project.
		// There should be some sort of message to the user before deleting??
		if ( bSuccess )
		{
			auto& pProjectInfo = MAIN_REGISTRY().GetContext<Scion::Core::ProjectInfoPtr>();
			SCION_ASSERT( pProjectInfo && "Project Info must exist!" );
			// Save entire project
			ProjectLoader pl{};
			if ( !pl.SaveLoadedProject( *pProjectInfo ) )
			{
				auto optProjectFilePath = pProjectInfo->GetProjectFilePath();
				SCION_ASSERT( optProjectFilePath && "Project file path not set correctly in project info." );

				SCION_ERROR( "Failed to save project [{}] at file [{}] after deleting asset [{}].",
							 pProjectInfo->GetProjectName(),
							 optProjectFilePath->string(),
							 sAssetName );
			}
		}
	}

	ImGui::SeparatorText( "File Explorer" );
	if ( ImGui::Selectable( ICON_FA_FILE_ALT " Open File Location" ) )
	{
		if ( m_eSelectedType == Scion::Utilities::AssetType::SCENE )
		{
			std::string sScenePath = SCENE_MANAGER().GetSceneFilepath( sAssetName );
			if ( !sScenePath.empty() )
			{
				Scion::Filesystem::FileProcessor fp{};
				if ( !fp.OpenFileLocation( Scion::Filesystem::NormalizePath( sScenePath ) ) )
				{
					SCION_ERROR( "Failed to open file location [{}]", sScenePath );
				}
			}
		}
		else
		{
			std::string sAssetPath = ASSET_MANAGER().GetAssetFilepath( sAssetName, m_eSelectedType );
			if ( !sAssetPath.empty() )
			{
				Scion::Filesystem::FileProcessor fp{};
				if ( !fp.OpenFileLocation( Scion::Filesystem::NormalizePath( sAssetPath ) ) )
				{
					SCION_ERROR( "Failed to open file location [{}]", sAssetPath );
				}
			}
		}
	}
}

void AssetDisplay::DrawSoundContext( const std::string& sAssetName )
{
	if ( m_eSelectedType == Scion::Utilities::AssetType::MUSIC )
	{
		ImGui::SeparatorText( "Music Controls" );
		auto& musicPlayer = MAIN_REGISTRY().GetMusicPlayer();

		if ( !musicPlayer.IsPlaying() )
		{
			if ( ImGui::Selectable( ICON_FA_PLAY " Play" ) )
			{
				auto& assetManager = MAIN_REGISTRY().GetAssetManager();
				if ( auto pMusic = assetManager.GetMusic( sAssetName ) )
				{
					musicPlayer.Play( *pMusic );
				}
			}

			ImGui::BeginDisabled();
			ImGui::Selectable( ICON_FA_STOP " Stop" );
			ImGui::EndDisabled();
		}
		else
		{
			ImGui::BeginDisabled();
			ImGui::Selectable( ICON_FA_PLAY " Play" );
			ImGui::EndDisabled();

			if ( ImGui::Selectable( ICON_FA_STOP " Stop" ) )
			{
				musicPlayer.Stop();
			}
		}
	}
	else if ( m_eSelectedType == Scion::Utilities::AssetType::SOUNDFX )
	{
		ImGui::SeparatorText( "Sound Controls" );
		auto& soundPlayer = MAIN_REGISTRY().GetSoundPlayer();

		if ( !soundPlayer.IsPlaying( 0 ) )
		{
			if ( ImGui::Selectable( ICON_FA_PLAY " Play" ) )
			{
				auto& assetManager = MAIN_REGISTRY().GetAssetManager();
				if ( auto pSoundfx = assetManager.GetSoundFx( sAssetName ) )
				{
					soundPlayer.Play( *pSoundfx, 0, 0 );
				}
			}

			ImGui::BeginDisabled();
			ImGui::Selectable( ICON_FA_STOP " Stop" );
			ImGui::EndDisabled();
		}
		else
		{
			ImGui::BeginDisabled();
			ImGui::Selectable( ICON_FA_PLAY " Play" );
			ImGui::EndDisabled();

			if ( ImGui::Selectable( ICON_FA_STOP " Stop" ) )
			{
				soundPlayer.Stop( 0 );
			}
		}
	}
}

void AssetDisplay::DrawSelectedAssets()
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	std::vector<std::string> assetNames{};

	if ( m_eSelectedType == Scion::Utilities::AssetType::SCENE )
	{
		assetNames = SCENE_MANAGER().GetSceneNames();
	}
	else
	{
		assetNames = assetManager.GetAssetKeyNames( m_eSelectedType );
	}

	if ( assetNames.empty() )
		return;

	const float avail = ImGui::GetContentRegionAvail().x; 
	const ImGuiStyle& style = ImGui::GetStyle();

	// one cell = image width + left/right cell padding
	const float cellW = m_AssetSize + style.CellPadding.x * 2.0f;

	// number of columns that *actually* fit in the visible region
	int numCols = (int)std::floor( ( avail + 0.5f ) / cellW );
	if ( numCols < 1 )
		numCols = 1;

	ImGuiTableFlags tableFlags{ 0 };
	tableFlags |= ImGuiTableFlags_SizingFixedFit;

	int k{ 0 }, id{ 0 };

	auto assetItr = assetNames.begin();

	if ( ImGui::BeginTable( "Assets", numCols, tableFlags ) )
	{
		for ( int c = 0; c < numCols; ++c )
			ImGui::TableSetupColumn( nullptr, ImGuiTableColumnFlags_WidthFixed, cellW );

		for (auto assetItr = assetNames.begin(); assetItr != assetNames.end(); ++assetItr, ++id)
		{
			ImGui::TableNextColumn();
			ImGui::PushID( k++ );

			bool bSelectedAsset = ( m_SelectedID == id );

			if ( bSelectedAsset )
				ImGui::TableSetBgColor( ImGuiTableBgTarget_CellBg,
										ImGui::GetColorU32( ImVec4{ 0.f, 0.9f, 0.f, 0.3f } ) );
			GLuint textureID = GetTextureID( *assetItr );
			std::string sCheckName{ m_sRenameBuf.data() };
			std::string assetBtn = "##asset" + std::to_string( id );

			if ( m_eSelectedType == Scion::Utilities::AssetType::PREFAB )
			{
				if ( auto pPrefab = assetManager.GetPrefab( *assetItr ) )
				{
					auto& sprite = pPrefab->GetPrefabbedEntity().sprite;
					if ( textureID && sprite )
					{
						ImGui::ImageButton(
							assetBtn.c_str(),
							(ImTextureID)(intptr_t)textureID,
							ImVec2{ m_AssetSize, m_AssetSize },
							ImVec2{ sprite->uvs.u, sprite->uvs.v },
							ImVec2{ sprite->uvs.u + sprite->uvs.uv_width, sprite->uvs.v + sprite->uvs.uv_height } );
					}
					else
					{
						ImGui::Button( assetBtn.c_str(), ImVec2{ m_AssetSize, m_AssetSize } );
					}
				}
			}
			else
			{
				if ( textureID == 0 )
				{
					ImGui::PopID();
					continue; 
				}

				ImGui::ImageButton(
					assetBtn.c_str(), (ImTextureID)(intptr_t)textureID, ImVec2{ m_AssetSize, m_AssetSize } );
			}

			if ( ImGui::IsItemHovered() && ImGui::IsMouseClicked( 0 ) && !m_bRename )
			{
				m_SelectedID = id;
			}

			const char* sAssetName = ( *assetItr ).c_str();

			if ( bSelectedAsset && ImGui::BeginPopupContextItem() )
			{
				OpenAssetContext( *assetItr );
				ImGui::EndPopup();
			}

			if ( ImGui::BeginDragDropSource() )
			{
				ImGui::SetDragDropPayload(
					m_sDragSource.c_str(), sAssetName, ( strlen( sAssetName ) + 1 ) * sizeof( char ), ImGuiCond_Once );
				ImGui::Image( (ImTextureID)(intptr_t)textureID, DRAG_ASSET_SIZE );
				ImGui::EndDragDropSource();
			}

			if ( !m_bRename || !bSelectedAsset )
			{
				ImGui::TextWrapped( sAssetName );
			}

			if ( m_bRename && bSelectedAsset )
			{
				ImGui::SetKeyboardFocusHere();
				if ( ImGui::InputText( "##rename", m_sRenameBuf.data(), 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
				{
					if ( !DoRenameAsset( *assetItr, sCheckName ) )
						SCION_ERROR( "Failed to change asset name." );

					m_sRenameBuf.clear();
					m_bRename = false;
				}
				else if ( m_bRename && ImGui::IsKeyPressed( ImGuiKey_Escape ) )
				{
					m_sRenameBuf.clear();
					m_bRename = false;
				}
			}

			if ( !m_bRename && bSelectedAsset && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( 0 ) )
			{
				m_sRenameBuf.clear();
				m_sRenameBuf = *assetItr;
				m_bRename = true;
			}

			if ( m_bRename && bSelectedAsset )
			{
				if ( std::string{ sAssetName } != sCheckName )
					CheckRename( sCheckName );
			}

			ImGui::PopID();
		}

		ImGui::EndTable();
	}

	// Click empty space to clear selection.
	if ( !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked( 0 ) && !m_bRename )
	{
		m_SelectedID = -1;
	}
}

AssetDisplay::AssetDisplay()
	: m_bAssetTypeChanged{ true }
	, m_bRename{ false }
	, m_bWindowSelected{ false }
	, m_bWindowHovered{ false }
	, m_bOpenAddAssetModal{ false }
	, m_sSelectedAssetName{}
	, m_sSelectedType{ "TEXTURES" }
	, m_sDragSource{}
	, m_sRenameBuf{}
	, m_eSelectedType{ Scion::Utilities::AssetType::TEXTURE }
	, m_AssetSize{ DEFAULT_ASSET_SIZE }
	, m_SelectedID{ -1 }
{
	SetAssetType();
}

void AssetDisplay::Draw()
{
	if ( auto& pEditorState = MAIN_REGISTRY().GetContext<EditorStatePtr>() )
	{
		if ( !pEditorState->IsDisplayOpen( EDisplay::AssetBrowser ) )
		{
			return;
		}
	}

	if ( !ImGui::Begin( ICON_FA_FILE_ALT " Assets" ) )
	{
		ImGui::End();
		return;
	}

	DrawToolbar();

	if ( ImGui::BeginChild( "##AssetTable",
							ImVec2{ 0.f, 0.f },
							ImGuiChildFlags_None,
							ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_ChildWindow ) )
	{
		m_bWindowHovered = ImGui::IsWindowHovered();
		m_bWindowSelected = ImGui::IsWindowFocused();

		DrawSelectedAssets();

		if ( m_SelectedID == -1 && ImGui::BeginPopupContextWindow( "##AddAsset" ) )
		{
			if ( ImGui::Selectable( AssetDisplayUtils::AddAssetBasedOnType( m_eSelectedType ).c_str() ) )
			{
				m_bOpenAddAssetModal = true;
			}

			ImGui::EndPopup();
		}

		if ( m_bOpenAddAssetModal )
			AssetDisplayUtils::OpenAddAssetModalBasedOnType( m_eSelectedType, &m_bOpenAddAssetModal );

		ImGui::EndChild();
	}

	ImGui::End();
}

void AssetDisplay::Update()
{
}

void AssetDisplay::DrawToolbar()
{

	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	ImGui::Separator();
	ImGui::PushStyleColor( ImGuiCol_Button, BLACK_TRANSPARENT );
	ImGui::PushStyleColor( ImGuiCol_ButtonHovered, BLACK_TRANSPARENT );
	ImGui::PushStyleColor( ImGuiCol_ButtonActive, BLACK_TRANSPARENT );
	ImGui::Button( "Asset Type" );
	ImGui::PopStyleColor( 3 );

	ImGui::SameLine( 0.f, 10.f );
	if ( ImGui::BeginCombo( "##AssetType", m_sSelectedType.c_str() ) )
	{

		for ( const auto& sAssetType : m_SelectableTypes )
		{
			bool bIsSelected = m_sSelectedType == sAssetType;
			if ( ImGui::Selectable( sAssetType.c_str(), bIsSelected ) )
			{
				m_bAssetTypeChanged = true;
				m_sSelectedType = sAssetType;
				m_SelectedID = -1;
				SetAssetType();
			}

			if ( bIsSelected )
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();
}

} // namespace Scion::Editor
