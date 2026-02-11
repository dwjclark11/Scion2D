#include "editor/displays/MenuDisplay.h"
#include "Logger/Logger.h"
#include "ScionFilesystem/Dialogs/FileDialog.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/Prefab.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/MainRegistry.h"

#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/tools/ToolManager.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/utilities/EditorState.h"
#include "Core/CoreUtilities/ProjectInfo.h"
#include "editor/loaders/ProjectLoader.h"
#include "editor/loaders/TiledMapImporter.h"

#include "Core/Events/EventDispatcher.h"
#include "editor/events/EditorEventTypes.h"

#include "ScionUtilities/ScionUtilities.h"

#include <imgui.h>
#include <SDL.h>

namespace Scion::Editor
{
void MenuDisplay::Draw()
{
	auto& sceneManager = SCENE_MANAGER();
	if ( ImGui::BeginMainMenuBar() )
	{
		if ( ImGui::BeginMenu( ICON_FA_FILE " File" ) )
		{
			ImGui::SeparatorText( "Project" );
			ImGui::InlineLabel( ICON_FA_FILE_ALT, 32.f );
			if ( ImGui::MenuItem( "New", "Ctrl + N" ) )
			{
				SCION_ERROR( "New -- Not Implemented" );
			}

			ImGui::InlineLabel( ICON_FA_FOLDER_OPEN, 32.f );
			if ( ImGui::MenuItem( "Open", "Ctrl + O" ) )
			{
				SCION_ERROR( "Open -- Not Implemented" );
			}
			ImGui::InlineLabel( ICON_FA_SAVE, 32.f );
			if ( ImGui::MenuItem( "Save All", "Ctrl + S" ) )
			{
				auto& pProjectInfo = MAIN_REGISTRY().GetContext<Scion::Core::ProjectInfoPtr>();
				SCION_ASSERT( pProjectInfo && "Project Info must exist!" );

				// Save entire project
				ProjectLoader pl{};
				if ( !pl.SaveLoadedProject( *pProjectInfo ) )
				{
					auto optProjectFilePath = pProjectInfo->GetProjectFilePath();
					SCION_ASSERT( optProjectFilePath && "Project file path not setup correctly." );
					SCION_ERROR( "Failed to save project [{}] at file [{}]",
								 pProjectInfo->GetProjectName(),
								 optProjectFilePath->string() );
				}
			}
			ImGui::ItemToolTip( "Saves entire project to disk." );

			ImGui::SeparatorText( "Scenes" );

			if ( auto pCurrentScene = sceneManager.GetCurrentScene() )
			{
				ImGui::InlineLabel( ICON_FA_SAVE, 32.f );
				if ( ImGui::MenuItem( "Save Current Scene As..." ) )
				{
					// TODO: Save a copy of the scene and all of it's entities under a new name.
				}
			}

			ImGui::SeparatorText( "Import" );
			ImGui::InlineLabel( ICON_FA_FILE_IMPORT, 32.f );
			if ( ImGui::MenuItem( "Import Tiled Map" ) )
			{
				Scion::Filesystem::FileDialog fd{};
				const auto sFilepath =
					fd.OpenFileDialog( "Import Tiled Map", BASE_PATH, { "*.lua", "*.tmx" }, "Tiled Map Files (*.lua, *.tmx)" );

				if ( !sFilepath.empty() )
				{
					if ( !TiledMapImporter::ImportTilemapFromTiled( &SCENE_MANAGER(), sFilepath ) )
					{
						SCION_ERROR( "Failed to import tiled map as new scene." );
					}
				}
			}

			ImGui::ItemToolTip( "Import a map from the Tiled Map Editor. Supports tmx and lua map files." );

			ImGui::SeparatorText( "Exit" );
			ImGui::InlineLabel( ICON_FA_WINDOW_CLOSE, 32.f );
			if ( ImGui::MenuItem( "Exit" ) )
			{
				EVENT_DISPATCHER().EmitEvent( Events::CloseEditorEvent{} );
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_EDIT " Edit" ) )
		{
			ImGui::SeparatorText( "Configuration" );

			auto& coreGlobals = CORE_GLOBALS();
			auto& toolManager = TOOL_MANAGER();

			bool bGridSnap{ toolManager.IsGridSnapEnabled() };
			if ( ImGui::Checkbox( "Enable Gridsnap", &bGridSnap ) )
			{
				toolManager.EnableGridSnap( bGridSnap );
			}

			bool bShowCollision{ coreGlobals.RenderCollidersEnabled() };
			if ( ImGui::Checkbox( "Show Collision", &bShowCollision ) )
			{
				bShowCollision ? coreGlobals.EnableColliderRender() : coreGlobals.DisableColliderRender();
			}

			bool bShowAnimations{ coreGlobals.AnimationRenderEnabled() };
			if ( ImGui::Checkbox( "Show Animations", &bShowAnimations ) )
			{
				bShowAnimations ? coreGlobals.EnableAnimationRender() : coreGlobals.DisableAnimationRender();
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_WINDOW_MAXIMIZE " Displays" ) )
		{
			ImGui::SeparatorText( "Displays" );

			auto& pEditorState = MAIN_REGISTRY().GetContext<EditorStatePtr>();
			DrawDisplayItem( *pEditorState, ICON_FA_FILE_ALT " Asset Browser", EDisplay::AssetBrowser );
			DrawDisplayItem( *pEditorState, ICON_FA_FOLDER " Content Browser", EDisplay::ContentBrowser );
			DrawDisplayItem( *pEditorState, ICON_FA_CODE " Script List", EDisplay::ScriptListView );
			DrawDisplayItem( *pEditorState, ICON_FA_ARCHIVE " Packager Game", EDisplay::PackagerView );
			DrawDisplayItem( *pEditorState, ICON_FA_TERMINAL " Console Logger", EDisplay::Console );
			DrawDisplayItem( *pEditorState, ICON_FA_COG " Project Settings", EDisplay::GameSettingsView );

			ImGui::Separator();
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_VIDEO " Scene" ) )
		{
			if ( auto pCurrentScene = SCENE_MANAGER().GetCurrentScene() )
			{
				ImGui::Text( "Current Scene" );
				ImGui::Separator();
				if ( ImGui::TreeNode( ICON_FA_FILE_IMAGE " Canvas" ) )
				{
					auto& canvas = pCurrentScene->GetCanvas();

					ImGui::InlineLabel( "width" );
					if ( ImGui::InputInt( "##_width", &canvas.width, canvas.tileWidth, canvas.tileWidth ) )
					{
						if ( canvas.width < 640 )
							canvas.width = 640;
					}
					ImGui::ItemToolTip( "Canvas Width - Clamped minimum = 640" );

					ImGui::InlineLabel( "height" );
					if ( ImGui::InputInt( "##_height", &canvas.height, canvas.tileHeight, canvas.tileHeight ) )
					{
						if ( canvas.height < 320 )
							canvas.height = 320;
					}
					ImGui::ItemToolTip( "Canvas Height - Clamped minimum = 320" );

					ImGui::InlineLabel( "tile width" );
					if ( ImGui::InputInt( "##tile_width", &canvas.tileWidth, 8, 8 ) )
					{
						canvas.tileWidth = std::clamp( canvas.tileWidth, 8, 128 );
					}
					ImGui::ItemToolTip( "Tile Width - Range [8 : 128]" );

					ImGui::InlineLabel( "tile height" );
					if ( ImGui::InputInt( "##tile_height", &canvas.tileHeight, 8, 8 ) )
					{
						canvas.tileHeight = std::clamp( canvas.tileHeight, 8, 128 );
					}
					ImGui::ItemToolTip( "Tile Height - Range [8 : 128]" );

					ImGui::TreePop();
				}
				ImGui::Separator();
				if ( ImGui::TreeNode( ICON_FA_COG " Settings" ) )
				{
					bool bChanged{ false };
					bool bPlayerStartEnabled{ pCurrentScene->IsPlayerStartEnabled() };
					ImGui::InlineLabel( "Enable Player Start:" );
					ImGui::SetCursorPosX( 250.f );

					ImGui::ItemToolTip(
						"Enable or Disable the player start.\n"
						"The player start is the character that we want to use when the scene is played." );

					if ( ImGui::Checkbox( "##_enablePlayerStart", &bPlayerStartEnabled ) )
					{
						pCurrentScene->SetPlayerStartEnabled( bPlayerStartEnabled );
						if ( bPlayerStartEnabled )
						{
							pCurrentScene->GetPlayerStart().LoadVisualEntity();
						}
						else
						{
							pCurrentScene->GetPlayerStart().Unload();
						}
					}

					std::string sPlayerStartCharacter{ pCurrentScene->GetPlayerStart().GetCharacterName() };
					auto prefabs =
						Scion::Utilities::GetKeys( ASSET_MANAGER().GetAllPrefabs() /*, []( auto& prefab ) {
			  return prefab.second->GetType() == Scion::Core::EPrefabType::Character;
		  } */ );

					ImGui::InlineLabel( ICON_FA_FLAG ICON_FA_GAMEPAD " Player Start Character:" );
					ImGui::SetCursorPosX( 250.f );
					ImGui::ItemToolTip( "The default player to spawn when starting the scene." );
					if ( ImGui::BeginCombo( "##DefaultPlayerStart", sPlayerStartCharacter.c_str() ) )
					{
						for ( const auto& sPrefabName : prefabs )
						{
							if ( ImGui::Selectable( sPrefabName.c_str(), sPrefabName == sPlayerStartCharacter ) )
							{
								sPlayerStartCharacter = sPrefabName;

								bChanged = true;
							}
						}

						ImGui::EndCombo();
					}

					if ( bChanged )
					{
						if ( auto pPrefab = ASSET_MANAGER().GetPrefab( sPlayerStartCharacter ) )
						{
							pCurrentScene->GetPlayerStart().SetCharacter( *pPrefab );
						}
					}

					bChanged = true;

					auto musicNames = Scion::Utilities::GetKeys( ASSET_MANAGER().GetAllMusic() );
					musicNames.push_back( "None" );

					std::string sDefaultSceneMusic{ pCurrentScene->GetDefaultMusicName() };
					if ( sDefaultSceneMusic.empty() )
						sDefaultSceneMusic = "None";

					ImGui::InlineLabel( ICON_FA_MUSIC " Default Music:" );
					ImGui::SetCursorPosX( 250.f );
					ImGui::ItemToolTip( "Music to play when the scene starts." );
					if ( ImGui::BeginCombo( "##DefaultMusic", sDefaultSceneMusic.c_str() ) )
					{
						for ( const auto& sMusicName : musicNames )
						{
							if ( ImGui::Selectable( sMusicName.c_str(), sMusicName == sDefaultSceneMusic ) )
							{
								sDefaultSceneMusic = sMusicName;
								bChanged = true;
							}
						}

						ImGui::EndCombo();
					}

					if ( bChanged )
					{
						pCurrentScene->SetDefaultMusic( sDefaultSceneMusic );
					}

					ImGui::TreePop();
				}
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_QUESTION_CIRCLE " Help" ) )
		{
			if ( ImGui::TreeNode( "About Scion2D" ) )
			{
				ImGui::Separator();
				ImGui::Text( "Scion2D is a simple 2D game engine and editor written in C++." );
				ImGui::Text( "The goal of the engine is to be able to quickly create games and learn more about C++ "
							 "Programming." );
				ImGui::Text( "By Dustin Clark and all contributors." );
				ImGui::AddSpaces( 2 );

				ImGui::SeparatorText( "Helpful Links: " );
				ImGui::TextLinkOpenURL( "Github", "https://github.com/dwjclark11/Scion2D" );
				ImGui::TextLinkOpenURL( "YouTube", "https://www.youtube.com/@JADE-iteGames" );
				ImGui::TextLinkOpenURL( "Documentation", "https://dwjclark11.github.io/Scion2D_Docs/" );
				ImGui::Separator();
				ImGui::TreePop();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void MenuDisplay::DrawDisplayItem( EditorState& editorState, const std::string& sDisplayName, const EDisplay eDisplay )
{
	bool bDisplayEnabled{ editorState.IsDisplayOpen( eDisplay ) };
	if ( ImGui::Selectable( sDisplayName.c_str(), false, ImGuiSelectableFlags_DontClosePopups ) )
	{
		bDisplayEnabled = !bDisplayEnabled;
		editorState.SetDisplay( eDisplay, bDisplayEnabled );
	}

	if ( bDisplayEnabled )
	{
		ImGui::SameLine();
		ImGui::SetCursorPosX( 150.f );
		ImGui::Text( ICON_FA_CHECK );
	}
}

} // namespace Scion::Editor
