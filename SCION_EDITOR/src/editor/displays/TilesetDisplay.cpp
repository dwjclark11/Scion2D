#include "TilesetDisplay.h"
#include "Logger/Logger.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/CoreUtilities/CoreUtilities.h"

#include "editor/scene/SceneManager.h"
#include "editor/tools/ToolManager.h"
#include "editor/tools/TileTool.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/utilities/EditorUtilities.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace SCION_EDITOR
{
void TilesetDisplay::DrawToolbar()
{
	auto& assetManager = ASSET_MANAGER();

	ImGui::Separator();
	if ( ImGui::Button( ICON_FA_PLUS_CIRCLE ) )
	{
		// TODO: Add new tileset functionality
	}
	ImGui::ItemToolTip( "Add Tileset" );
	ImGui::SameLine();

	ImGui::PushStyleColor( ImGuiCol_Button, BLACK_TRANSPARENT );
	ImGui::PushStyleColor( ImGuiCol_ButtonHovered, BLACK_TRANSPARENT );
	ImGui::PushStyleColor( ImGuiCol_ButtonActive, BLACK_TRANSPARENT );
	ImGui::Button( "Choose Tileset" );
	ImGui::PopStyleColor( 3 );

	ImGui::SameLine();
	if ( ImGui::BeginCombo( "##Choose_Tileset", m_sTileset.c_str() ) )
	{
		for ( const auto& sTileset : assetManager.GetTilesetNames() )
		{
			bool bIsSelected = m_sTileset == sTileset;
			if ( ImGui::Selectable( sTileset.c_str(), bIsSelected ) )
			{
				m_sTileset = sTileset;
				SCENE_MANAGER().SetTileset( sTileset );
			}

			if ( bIsSelected )
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
	ImGui::Separator();
}

void TilesetDisplay::Draw()
{
	if ( !ImGui::Begin( "Tileset" ) )
	{
		ImGui::End();
		return;
	}

	DrawToolbar();

	if ( m_sTileset.empty() )
	{
		ImGui::End();
		return;
	}

	auto pTexture = ASSET_MANAGER().GetTexture( m_sTileset );
	if ( !pTexture )
	{
		ImGui::End();
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	bool bMouseHeld{ ImGui::IsMouseDown( ImGuiMouseButton_Left ) };
	bool bMouseReleased{ ImGui::IsMouseReleased( ImGuiMouseButton_Left ) };

	// TODO: Get the tile sizes from the Scene's Canvas
	int tileWidth{ 16 };
	int tileHeight{ 16 };

	float textureWidth = static_cast<float>( pTexture->GetWidth() );
	float textureHeight = static_cast<float>( pTexture->GetHeight() );

	const int COLS = textureWidth / tileWidth;
	const int ROWS = textureHeight / tileHeight;

	ImGuiTableFlags tableFlags{ 0 };
	tableFlags |= ImGuiTableFlags_SizingFixedFit;
	tableFlags |= ImGuiTableFlags_ScrollX;

	int id{ 0 };

	if ( ImGui::BeginTable( "Tileset", COLS, tableFlags ) )
	{
		for ( int row = 0; row < ROWS; row++ )
		{
			ImGui::TableNextRow();
			for ( int col = 0; col < COLS; col++ )
			{
				ImGui::TableSetColumnIndex( col );

				// Create unique id for the buttons
				id = row * COLS + col;
				ImGui::PushID( id );
				std::string buttonStr = "##tile_" + std::to_string( id );

				// Get UV coordinates for this cell
				float u0 = col * tileWidth / textureWidth;
				float v0 = row * tileHeight / textureHeight;
				float u1 = ( col + 1 ) * tileWidth / textureWidth;
				float v1 = ( row + 1 ) * tileHeight / textureHeight;

				ImVec2 cellMin = ImGui::GetCursorScreenPos();
				ImVec2 cellMax = { cellMin.x + tileWidth, cellMin.y + tileHeight };

				if ( bMouseHeld && ImGui::IsMouseHoveringRect( cellMin, cellMax ) && m_Selection.IsValid() &&
					 m_Selection.bSelecting )
				{
					m_Selection.endRow = row;
					m_Selection.endCol = col;
				}
				else if ( bMouseReleased )
				{
					m_Selection.bSelecting = false;
				}

				bool bSelected{ false };
				if ( m_Selection.IsValid() )
				{
					int minRow = std::min( m_Selection.startRow, m_Selection.endRow );
					int maxRow = std::max( m_Selection.startRow, m_Selection.endRow );
					int minCol = std::min( m_Selection.startCol, m_Selection.endCol );
					int maxCol = std::max( m_Selection.startCol, m_Selection.endCol );

					bSelected = ( row >= minRow && row <= maxRow && col >= minCol && col <= maxCol );

					if ( auto pActiveTool = TOOL_MANAGER().GetActiveTool(); bMouseHeld )
					{
						auto& tileData = pActiveTool->GetTileData();

						tileData.sprite.width = ( std::abs( maxCol - minCol ) + 1 ) * tileWidth;
						tileData.sprite.height = ( std::abs( maxRow - minRow ) + 1 ) * tileHeight;

						SCION_CORE::GenerateUVsExt( tileData.sprite,
													textureWidth,
													textureHeight,
													minCol * tileWidth / textureWidth,
													minRow * tileHeight / textureHeight );
					}
				}

				ImVec4 tintColor = bSelected ? ImVec4{ 0.3f, 0.6f, 1.f, 1.f } : ImVec4{ 1.f, 1.f, 1.f, 1.f };
				ImVec4 borderColor = bSelected ? ImVec4{ 1.f, 1.f, 1.f, 1.f } : ImVec4{ 0.f, 0.f, 0.f, 0.f };

				if ( bSelected )
				{
					ImGui::TableSetBgColor( ImGuiTableBgTarget_CellBg,
											ImGui::GetColorU32( ImVec4{ 0.f, 0.9f, 0.f, 0.2f } ) );
				}

				if ( ImGui::ImageButtonEx( ImGui::GetID( fmt::format( "##ImageBtn_{}", id ).c_str() ),
										   (ImTextureID)(intptr_t)pTexture->GetID(),
										   ImVec2{ static_cast<float>( tileWidth ), static_cast<float>( tileHeight ) },
										   ImVec2{ u0, v0 },
										   ImVec2{ u1, v1 },
										   borderColor,
										   tintColor,
										   ImGuiButtonFlags_PressedOnClick ) )
				{
					m_Selection.Reset();
					m_Selection.startRow = m_Selection.endRow = row;
					m_Selection.startCol = m_Selection.endCol = col;
					m_Selection.bSelecting = true;
				}

				ImGui::PopID();
			}
		}
		ImGui::EndTable();
	}

	ImGui::End();
}
} // namespace SCION_EDITOR
