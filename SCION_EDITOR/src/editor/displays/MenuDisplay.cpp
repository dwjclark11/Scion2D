#include "MenuDisplay.h"
#include "Logger/Logger.h"
#include "ScionFilesystem/Dialogs/FileDialog.h"
#include "Core/Loaders/TilemapLoader.h"
#include "Core/CoreUtilities/CoreEngineData.h"

#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/tools/ToolManager.h"

#include <imgui.h>
#include <SDL.h>

namespace SCION_EDITOR
{
void MenuDisplay::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "Ctrl + N"))
			{
				SCION_LOG( "NEW PRESSED" );
			}

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

			if ( ImGui::MenuItem( "Exit") )
			{
				SCION_LOG( "SHOULD EVENTUALLY EXIT!" );				
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Edit" ) )
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

		if ( ImGui::BeginMenu( "Tools" ) )
		{
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Settings" ) )
		{
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Help" ) )
		{
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
} // namespace SCION_EDITOR
