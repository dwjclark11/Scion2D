#include "AssetDisplay.h"
#include "AssetDisplayUtils.h"
#include "ScionUtilities/ScionUtilities.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Scripting/InputManager.h"
#include "Core/Resources/AssetManager.h"
#include "Logger/Logger.h"

#include "editor/utilities/EditorUtilities.h"
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

#include <imgui.h>

constexpr float DEFAULT_ASSET_SIZE = 64.f;
constexpr ImVec2 DRAG_ASSET_SIZE = ImVec2{ 32.f, 32.f };

namespace SCION_EDITOR
{
void AssetDisplay::SetAssetType()
{
	if ( !m_bAssetTypeChanged )
		return;

	if ( m_sSelectedType == "TEXTURES" )
	{
		m_eSelectedType = SCION_UTIL::AssetType::TEXTURE;
		m_sDragSource = std::string{ DROP_TEXTURE_SRC };
	}
	else if ( m_sSelectedType == "FONTS" )
	{
		m_eSelectedType = SCION_UTIL::AssetType::FONT;
		m_sDragSource = std::string{ DROP_FONT_SRC };
	}
	else if ( m_sSelectedType == "SOUNDFX" )
	{
		m_eSelectedType = SCION_UTIL::AssetType::SOUNDFX;
		m_sDragSource = std::string{ DROP_SOUNDFX_SRC };
	}
	else if ( m_sSelectedType == "MUSIC" )
	{
		m_eSelectedType = SCION_UTIL::AssetType::MUSIC;
		m_sDragSource = std::string{ DROP_MUSIC_SRC };
	}
	else if ( m_sSelectedType == "SCENES" )
	{
		m_eSelectedType = SCION_UTIL::AssetType::SCENE;
		m_sDragSource = std::string{ DROP_SCENE_SRC };
	}
	else if ( m_sSelectedType == "PREFABS" )
	{
		m_eSelectedType = SCION_UTIL::AssetType::PREFAB;
		m_sDragSource = std::string{ DROP_PREFAB_SRC };
	}
	else
	{
		m_eSelectedType = SCION_UTIL::AssetType::NO_TYPE;
		m_sDragSource = "NO_ASSET_TYPE";
	}

	m_bAssetTypeChanged = false;
}

unsigned int AssetDisplay::GetTextureID( const std::string& sAssetName ) const
{
	auto& assetManager = MAIN_REGISTRY().GetAssetManager();
	switch ( m_eSelectedType )
	{
	case SCION_UTIL::AssetType::TEXTURE: {
		auto pTexture = assetManager.GetTexture( sAssetName );
		if ( pTexture )
			return pTexture->GetID();

		break;
	}
	case SCION_UTIL::AssetType::FONT: {
		auto pFont = assetManager.GetFont( sAssetName );
		if ( pFont )
			return pFont->GetFontAtlasID();

		break;
	}
	case SCION_UTIL::AssetType::SOUNDFX:
	case SCION_UTIL::AssetType::MUSIC: {
		auto pTexture = assetManager.GetTexture( "music_icon" );
		if ( pTexture )
			return pTexture->GetID();

		break;
	}
	case SCION_UTIL::AssetType::SCENE: {
		auto pTexture = assetManager.GetTexture( "scene_icon" );
		if ( pTexture )
			return pTexture->GetID();

		break;
	}
	case SCION_UTIL::AssetType::PREFAB: {
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

	if ( m_eSelectedType == SCION_UTIL::AssetType::SCENE )
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

	if ( m_eSelectedType == SCION_UTIL::AssetType::SCENE )
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
	if ( ImGui::Selectable( "rename" ) )
	{
		m_bRename = true;
	}

	if ( ImGui::Selectable( "delete" ) )
	{
		bool bSuccess{ false };
		if ( m_eSelectedType == SCION_UTIL::AssetType::SCENE )
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
			auto& pProjectInfo = MAIN_REGISTRY().GetContext<SCION_CORE::ProjectInfoPtr>();
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
}

void AssetDisplay::DrawSelectedAssets()
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	std::vector<std::string> assetNames{};

	if ( m_eSelectedType == SCION_UTIL::AssetType::SCENE )
	{
		assetNames = SCENE_MANAGER().GetSceneNames();
	}
	else
	{
		assetNames = assetManager.GetAssetKeyNames( m_eSelectedType );
	}

	if ( assetNames.empty() )
		return;

	float windowWidth = ImGui::GetWindowWidth();

	int numCols = static_cast<int>( ( windowWidth - m_AssetSize ) / m_AssetSize );
	int numRows = static_cast<int>( assetNames.size() / ( numCols <= 1 ? 1 : numCols ) + 1 );

	if ( !numCols || !numRows )
		return;

	ImGuiTableFlags tableFlags{ 0 };
	tableFlags |= ImGuiTableFlags_SizingFixedFit;

	int k{ 0 }, id{ 0 };

	auto assetItr = assetNames.begin();

	if ( ImGui::BeginTable( "Assets", numCols, tableFlags ) )
	{
		for ( int row = 0; row < numRows; row++ )
		{
			ImGui::TableNextRow();
			for ( int col = 0; col < numCols; col++ )
			{
				if ( assetItr == assetNames.end() )
					break;

				ImGui::PushID( k++ );
				ImGui::TableSetColumnIndex( col );

				bool bSelectedAsset{ m_SelectedID == id };

				if ( bSelectedAsset )
					ImGui::TableSetBgColor( ImGuiTableBgTarget_CellBg,
											ImGui::GetColorU32( ImVec4{ 0.f, 0.9f, 0.f, 0.3f } ) );

				GLuint textureID{ GetTextureID( *assetItr ) };
				std::string sCheckName{ m_sRenameBuf.data() };

				std::string assetBtn = "##asset" + std::to_string( id );

				if ( m_eSelectedType == SCION_UTIL::AssetType::PREFAB )
				{
					// TODO: We are currently assuming that all prefabs will have a sprite component.
					// We need to create an engine/editor texture that will be used in case of the prefab
					// not having a sprite.
					if ( auto pPrefab = assetManager.GetPrefab( *assetItr ) )
					{
						auto& sprite = pPrefab->GetPrefabbedEntity().sprite;
						if ( textureID && sprite )
						{
							ImGui::ImageButton( assetBtn.c_str(),
												(ImTextureID)(intptr_t)textureID,
												ImVec2{ m_AssetSize, m_AssetSize },
												ImVec2{ sprite->uvs.u, sprite->uvs.v },
												ImVec2{ sprite->uvs.uv_width, sprite->uvs.uv_height } );
							
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
						break;
					}

					ImGui::ImageButton(
						assetBtn.c_str(), (ImTextureID)(intptr_t)textureID, ImVec2{ m_AssetSize, m_AssetSize } );
				}

				if ( ImGui::IsItemHovered() && ImGui::IsMouseClicked( 0 ) && !m_bRename )
					m_SelectedID = id;

				auto sAssetName = ( *assetItr ).c_str();

				if ( bSelectedAsset && ImGui::BeginPopupContextItem() )
				{
					OpenAssetContext( *assetItr );

					ImGui::EndPopup();
				}

				if ( ImGui::BeginDragDropSource() )
				{
					ImGui::SetDragDropPayload( m_sDragSource.c_str(),
											   sAssetName,
											   ( strlen( sAssetName ) + 1 ) * sizeof( char ),
											   ImGuiCond_Once );

					ImGui::Image( (ImTextureID)(intptr_t)textureID, DRAG_ASSET_SIZE );
					ImGui::EndDragDropSource();
				}

				if ( !m_bRename || !bSelectedAsset )
					ImGui::Text( sAssetName );

				if ( m_bRename && bSelectedAsset )
				{
					ImGui::SetKeyboardFocusHere();
					if ( ImGui::InputText(
							 "##rename", m_sRenameBuf.data(), 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
					{
						if ( !DoRenameAsset( *assetItr, sCheckName ) )
						{
							SCION_ERROR( "Failed to change asset name." );
						}

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
					if ( sAssetName != sCheckName )
						CheckRename( sCheckName );
				}

				++id;
				++assetItr;
				ImGui::PopID();
			}
		}

		ImGui::EndTable();
	}

	// If we are clicking on the display and no item, we want to reset the selection.
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
	, m_eSelectedType{ SCION_UTIL::AssetType::TEXTURE }
	, m_AssetSize{ DEFAULT_ASSET_SIZE }
	, m_SelectedID{ -1 }
{
	SetAssetType();
}

void AssetDisplay::Draw()
{
	if ( !ImGui::Begin( "Assets" ) )
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

} // namespace SCION_EDITOR
