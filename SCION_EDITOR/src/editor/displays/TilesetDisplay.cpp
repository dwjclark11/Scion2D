#include "editor/displays/TilesetDisplay.h"
#include "Logger/Logger.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/tools/ToolManager.h"
#include "editor/tools/TileTool.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"

#include <Rendering/Essentials/Texture.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace Scion::Editor
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
	if ( !ImGui::Begin( ICON_FA_TH " Tileset" ) )
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

	// TODO: Get the tile width from the canvas
	const int tileWidth{ 16 };
	const int tileHeight{ 16 };

	const float textureWidth = static_cast<float>( pTexture->GetWidth() );
	const float textureHeight = static_cast<float>( pTexture->GetHeight() );

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

				// UV Coordinates for this cell
				float u0 = col * tileWidth / textureWidth;
				float v0 = row * tileHeight / textureHeight;
				float u1 = ( col + 1 ) * tileWidth / textureWidth;
				float v1 = ( row + 1 ) * tileHeight / textureHeight;

				// Get the cursor position for the selection
				ImVec2 cellMin = ImGui::GetCursorScreenPos();
				ImVec2 cellMax = { cellMin.x + tileWidth, cellMin.y + tileHeight };

				// Detect Hove and Selection
				if ( ImGui::IsMouseHoveringRect( cellMin, cellMax ) && bMouseHeld && m_TableSelection.IsValid() )
				{
					m_TableSelection.endRow = row;
					m_TableSelection.endCol = col;
				}
				else if ( bMouseReleased )
				{
					m_TableSelection.bSelecting = false;
				}

				// Ensure selection remains rectangular
				bool bSelected{ false };
				if ( m_TableSelection.IsValid() )
				{
					int minRow = std::min( m_TableSelection.startRow, m_TableSelection.endRow );
					int maxRow = std::max( m_TableSelection.startRow, m_TableSelection.endRow );
					int minCol = std::min( m_TableSelection.startCol, m_TableSelection.endCol );
					int maxCol = std::max( m_TableSelection.startCol, m_TableSelection.endCol );

					bSelected = ( row >= minRow && row <= maxRow && col >= minCol && col <= maxCol );

					if ( auto pActiveTool = TOOL_MANAGER().GetActiveTool() )
					{
						auto& tileData = pActiveTool->GetTileData();
						tileData.sprite.width = ( std::abs( maxCol - minCol ) + 1 ) * tileWidth;
						tileData.sprite.height = ( std::abs( maxRow - minRow ) + 1 ) * tileHeight;
						tileData.sprite.start_x = minCol;
						tileData.sprite.start_y = minRow;

						// We only want to do this when the selection changes!!
						Scion::Core::GenerateUVsExt( tileData.sprite,
													textureWidth,
													textureHeight,
													minCol * tileWidth / textureWidth,
													minRow * tileHeight / textureHeight );
					}
				}

				// Visual Effects for the selection
				ImVec4 tintColor = bSelected ? ImVec4{ 0.3f, 0.6f, 1.f, 1.f } : ImVec4{ 1.f, 1.f, 1.f, 1.f };
				ImVec4 borderColor = bSelected ? ImVec4{ 1.f, 1.f, 1.f, 1.f } : ImVec4{ 0.f, 0.f, 0.f, 0.f };

				if ( ImGui::ImageButtonEx( ImGui::GetID( fmt::format( "##ImageBtn_{}", id ).c_str() ),
										   (ImTextureID)(intptr_t)pTexture->GetID(),
										   ImVec2{ static_cast<float>( tileWidth ), static_cast<float>( tileHeight ) },
										   ImVec2{ u0, v0 },
										   ImVec2{ u1, v1 },
										   borderColor,
										   tintColor,
										   ImGuiButtonFlags_PressedOnClick ) )
				{
					m_TableSelection.Reset();
					m_TableSelection.startRow = m_TableSelection.endRow = row;
					m_TableSelection.startCol = m_TableSelection.endCol = col;
					m_TableSelection.bSelecting = true;
				}

				ImGui::PopID();
			}
		}
		ImGui::EndTable();
	}

	ImGui::End();
}
} // namespace Scion::Editor
