#include "TilesetDisplay.h"
#include "Logger/Logger.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/MainRegistry.h"

#include <imgui.h>

namespace SCION_EDITOR
{
void TilesetDisplay::Draw()
{
	if ( !ImGui::Begin( "Tileset" ) )
	{
		ImGui::End();
		return;
	}

	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	if ( ImGui::BeginCombo( "Choose Tileset", m_sTileset.c_str() ) )
	{
		for ( const auto& sTileset : assetManager.GetTilesetNames() )
		{
			bool bIsSelected = m_sTileset == sTileset;
			if ( ImGui::Selectable( sTileset.c_str(), bIsSelected ) )
				m_sTileset = sTileset;

			if ( bIsSelected )
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	if ( m_sTileset.empty() )
	{
		ImGui::End();
		return;
	}

	auto pTexture = assetManager.GetTexture( m_sTileset );
	if ( !pTexture )
	{
		ImGui::End();
		return;
	}

	int textureWidth = pTexture->GetWidth();
	int textureHeight = pTexture->GetHeight();

	int cols = textureWidth / 16;
	int rows = textureHeight / 16;

	float uv_w = 16 / static_cast<float>( textureWidth );
	float uv_h = 16 / static_cast<float>( textureHeight );

	float ux{ 0.f }, uy{ 0.f }, vx{ uv_w }, vy{ uv_h };

	ImGuiTableFlags tableFlags{ 0 };
	tableFlags |= ImGuiTableFlags_SizingFixedFit;
	tableFlags |= ImGuiTableFlags_ScrollX;

	int k{ 0 }, id{ 0 };

	if (ImGui::BeginTable("Tileset", cols, tableFlags))
	{
		for (int i = 0; i < rows; i++)
		{
			ImGui::TableNextRow();
			for ( int j = 0; j < cols; j++ )
			{
				ImGui::TableSetColumnIndex( j );

				if ( m_Selected == id )
					ImGui::TableSetBgColor( ImGuiTableBgTarget_CellBg,
											ImGui::GetColorU32( ImVec4{ 0.f, 0.9f, 0.f, 0.3f } ) );

				// Create unique id for the buttons
				ImGui::PushID( k++ );

				if (ImGui::ImageButton((ImTextureID)pTexture->GetID(), ImVec2{ 16.f * 1.5, 16.f * 1.5, }, ImVec2{ ux, uy }, ImVec2{ vx, vy }))
				{
					m_Selected = id;
					SCION_LOG( "StartX: {}, StartY: {}", j, i );
				}

				ImGui::PopID();

				// Advance the UVs to the next column
				ux += uv_w;
				vx += uv_w;
				++id;
			}
			// Put the UVs back to the start column of the next row 
			ux = 0.f;
			vx = uv_w;
			uy += uv_h;
			vy += uv_h;
		}
		ImGui::EndTable();
	}

	ImGui::End();
}
} // namespace SCION_EDITOR
