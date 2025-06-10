#include "MenuDisplay.h"
#include "Logger/Logger.h"
#include "ScionFilesystem/Dialogs/FileDialog.h"
#include "Core/Loaders/TilemapLoader.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/Prefab.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/MainRegistry.h"

#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/tools/ToolManager.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "Core/CoreUtilities/SaveProject.h"
#include "editor/loaders/ProjectLoader.h"

#include "Core/Events/EventDispatcher.h"
#include "editor/events/EditorEventTypes.h"

#include "ScionUtilities/ScionUtilities.h"

#include <imgui.h>
#include <SDL.h>

namespace SCION_EDITOR
{
void MenuDisplay::Draw()
{
	if ( ImGui::BeginMainMenuBar() )
	{
		if ( ImGui::BeginMenu( ICON_FA_FILE " File" ) )
		{
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
			if ( ImGui::MenuItem( "Save", "Ctrl + S" ) )
			{
				auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<SCION_CORE::SaveProject>>();
				SCION_ASSERT( pSaveProject && "Save Project must exist!" );
				// Save entire project
				ProjectLoader pl{};
				if ( !pl.SaveLoadedProject( *pSaveProject ) )
				{
					SCION_ERROR( "Failed to save project [{}] at file [{}]",
								 pSaveProject->sProjectName,
								 pSaveProject->sProjectFilePath );
				}
			}

			ImGui::InlineLabel( ICON_FA_WINDOW_CLOSE, 32.f );
			if ( ImGui::MenuItem( "Exit" ) )
			{
				EVENT_DISPATCHER().EmitEvent( Events::CloseEditorEvent{} );
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
			
			static bool bShowAnimations{ true };
			if ( ImGui::Checkbox("Show Animations", &bShowAnimations))
			{
				if ( bShowAnimations )
					coreGlobals.EnableAnimationRender();
				else
					coreGlobals.DisableAnimationRender();
			}

			if ( ImGui::TreeNode( "Project Settings" ) )
			{
				// TODO: Add specific Project settings
				/*
				 * Desired Settings
				 * - Window Size
				 * - Window Position
				 * - Window flags
				 *
				 */
				ImGui::TreePop();
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_WINDOW_MAXIMIZE " Displays" ) )
		{
			// TODO: Open and close specific displays

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_VIDEO " Scene" ) )
		{
			if ( auto pCurrentScene = SCENE_MANAGER().GetCurrentScene() )
			{
				ImGui::Text( "Current Scene" );
				ImGui::Separator();
				if ( ImGui::TreeNode( "Canvas" ) )
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
				if ( ImGui::TreeNode( "Settings" ) )
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
						SCION_UTIL::GetKeys( ASSET_MANAGER().GetAllPrefabs() /*, []( auto& prefab ) {
			  return prefab.second->GetType() == SCION_CORE::EPrefabType::Character;
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

					auto musicNames = SCION_UTIL::GetKeys( ASSET_MANAGER().GetAllMusic() );
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

		if ( ImGui::BeginMenu( ICON_FA_COG " Settings" ) )
		{
			if ( ImGui::TreeNode( "Physics" ) )
			{
				ImGui::Separator();
				auto& coreGlobals = CORE_GLOBALS();
				bool bEnablePhysics = coreGlobals.IsPhysicsEnabled();
				ImGui::InlineLabel( "Enable Physics", 176.f );
				if ( ImGui::Checkbox( "##enable_physics", &bEnablePhysics ) )
				{
					if ( bEnablePhysics )
						coreGlobals.EnablePhysics();
					else
						coreGlobals.DisablePhysics();
				}

				int32 velocityIterations = coreGlobals.GetVelocityIterations();
				int32 positionIterations = coreGlobals.GetPositionIterations();
				float gravity = coreGlobals.GetGravity();
				ImGui::InlineLabel( "Gravity", 176.f );
				if ( ImGui::InputFloat( "##Gravity", &gravity, .1f, .1f, "%.1f" ) )
				{
					coreGlobals.SetGravity( gravity );
				}

				ImGui::InlineLabel( "Velocity Iterations", 176.f );
				if ( ImGui::InputInt( "##VelocityIterations", &velocityIterations, 1, 1 ) )
				{
					coreGlobals.SetVelocityIterations( velocityIterations );
				}

				ImGui::InlineLabel( "Position Iterations", 176.f );
				if ( ImGui::InputInt( "##PositionIterations", &positionIterations, 1, 1 ) )
				{
					coreGlobals.SetPositionIterations( positionIterations );
				}
				ImGui::TreePop();
			}

			auto& coreGlobals = CORE_GLOBALS();
			bool bChanged{ false };
			std::string sGameType{ coreGlobals.GetGameTypeStr( coreGlobals.GetGameType() ) };
			SCION_CORE::EGameType eGameType{ coreGlobals.GetGameType() };

			ImGui::InlineLabel( ICON_FA_GAMEPAD " Game Type:" );
			ImGui::SetCursorPosX( 250.f );
			ImGui::ItemToolTip( "The type of game this is going to be." );

			if ( ImGui::BeginCombo( "##DefaultMusic", sGameType.c_str() ) )
			{
				for ( const auto& [ eType, sTypeStr ] : coreGlobals.GetGameTypesMap() )
				{
					if ( ImGui::Selectable( sTypeStr.c_str(), sTypeStr == sGameType ) )
					{
						sGameType = sTypeStr;
						eGameType = eType;
						bChanged = true;
					}
				}

				ImGui::EndCombo();
			}

			if ( bChanged )
			{
				coreGlobals.SetGameType( eGameType );
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
				ImGui::Text( "Helpful Links: " );
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
} // namespace SCION_EDITOR
