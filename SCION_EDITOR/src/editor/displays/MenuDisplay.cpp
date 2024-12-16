#include "MenuDisplay.h"
#include "Logger/Logger.h"
#include "ScionFilesystem/Dialogs/FileDialog.h"
#include "Core/Loaders/TilemapLoader.h"
#include "Core/CoreUtilities/CoreEngineData.h"

#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/tools/ToolManager.h"
#include "editor/utilities/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"

#include <imgui.h>
#include <SDL.h>

namespace SCION_EDITOR
{
void MenuDisplay::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if ( ImGui::BeginMenu( ICON_FA_FILE " File" ) )
		{
			ImGui::InlineLabel( ICON_FA_FILE_ALT, 32.f );
			if (ImGui::MenuItem("New", "Ctrl + N"))
			{
				SCION_LOG( "NEW PRESSED" );
			}

			ImGui::InlineLabel( ICON_FA_FOLDER_OPEN, 32.f );
			if ( ImGui::MenuItem( "Open", "Ctrl + O" ) )
			{
				SCION_FILESYSTEM::FileDialog fd{};
				auto file = fd.OpenFileDialog( "Open tilemap test", SDL_GetBasePath(), { "*.json" } );

				if (!file.empty())
				{
					auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
					if ( pCurrentScene )
					{
						SCION_CORE::Loaders::TilemapLoader tl{};
						if ( !tl.LoadTilemap( pCurrentScene->GetRegistry(), file, true ) )
						{
							SCION_ERROR( "Failed to load tilemap." );
						}
					}
					else
					{
						SCION_ERROR( "Failed to load tilemap. No active scene." );
					}
				}
			}
			ImGui::InlineLabel( ICON_FA_SAVE, 32.f );
			if ( ImGui::MenuItem( "Save", "Ctrl + S" ) )
			{
				SCION_FILESYSTEM::FileDialog fd{};
				auto file = fd.SaveFileDialog( "Save Tilemap test", SDL_GetBasePath(), { "*.json" } );
				if (!file.empty())
				{
					auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
					if (pCurrentScene)
					{
						SCION_CORE::Loaders::TilemapLoader tl{};
						if (!tl.SaveTilemap(pCurrentScene->GetRegistry(), file, true))
						{
							SCION_ERROR( "Failed to save tilemap." );
						}
					}
					else
					{
						SCION_ERROR( "Failed to save tilemap. No active scene." );
					}
				}
			}

			ImGui::InlineLabel( ICON_FA_WINDOW_CLOSE, 32.f );
			if ( ImGui::MenuItem("Exit") )
			{
				SCION_LOG( "SHOULD EVENTUALLY EXIT!" );				
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_EDIT " Edit" ) )
		{
			auto& coreGlobals = CORE_GLOBALS();

			static bool bGridSnap{ true };
			if ( ImGui::Checkbox( "Enable Gridsnap", &bGridSnap ) )
			{
				SCENE_MANAGER().GetToolManager().EnableGridSnap( bGridSnap );
			}

			static bool bShowCollision{ false };
			if ( ImGui::Checkbox( "Show Collision", &bShowCollision ) )
			{
				if ( bShowCollision )
					coreGlobals.EnableColliderRender();
				else
					coreGlobals.DisableColliderRender();
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_TOOLS " Tools" ) )
		{
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_COG " Settings" ) )
		{
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_QUESTION_CIRCLE " Help" ) )
		{
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
} // namespace SCION_EDITOR
