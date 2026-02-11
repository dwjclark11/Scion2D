#include "editor/displays/ProjectSettingsDisplay.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/ProjectInfo.h"
#include "Core/ECS/MainRegistry.h"
#include "Physics/PhysicsUtilities.h"
#include "ScionFilesystem/Dialogs/FileDialog.h"
#include "Rendering/Essentials/Texture.h"
#include "Logger/Logger.h"

#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/EditorState.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/scene/SceneManager.h"

#include <imgui.h>
#include <imgui_stdlib.h>

using namespace Scion::Physics;
using namespace Scion::Filesystem;

namespace Scion::Editor
{
ProjectSettingsDisplay::ProjectSettingsDisplay()
	: m_SettingsCategory{}
	, m_sSelectedCategory{ "General" }
{
	CreateProjectSettings();
}

ProjectSettingsDisplay::~ProjectSettingsDisplay() = default;

void ProjectSettingsDisplay::Update()
{
}
void ProjectSettingsDisplay::Draw()
{
	if ( auto& pEditorState = MAIN_REGISTRY().GetContext<EditorStatePtr>() )
	{
		if ( !pEditorState->IsDisplayOpen( EDisplay::GameSettingsView ) )
		{
			return;
		}
	}

	ImGui::Begin( ICON_FA_COG " Project Settings" );
	const float leftWidth = 250.0f;

	ImGui::Columns( 2, nullptr, true );
	ImGui::SetColumnWidth( 0, leftWidth );

	// Create two child regions side-by-side
	ImGui::BeginChild( "SettingsNav", ImVec2( leftWidth, 0 ), true ); // Left: category tree
	DrawCategoryTree( m_SettingsCategory, m_sSelectedCategory );
	ImGui::EndChild();

	ImGui::NextColumn();

	ImGui::BeginChild( "SettingsPanel", ImGui::GetContentRegionAvail(), true ); // Right: settings panel
	DrawSettingsPanel( m_SettingsCategory, m_sSelectedCategory );
	ImGui::EndChild();

	ImGui::Columns( 1 );

	ImGui::End();
}

void ProjectSettingsDisplay::DrawCategoryTree( const SettingCategory& category, std::string& selected )
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if ( !category.subCategories.empty() )
	{
		bool bOpen = ImGui::TreeNodeEx( category.sName.c_str(), flags );
		if ( ImGui::IsItemClicked() )
		{
			selected = category.sName;
		}

		if ( bOpen )
		{
			for ( const auto& sub : category.subCategories )
			{
				DrawCategoryTree( sub, selected );
			}
			ImGui::TreePop();
		}
	}
	else
	{
		if ( ImGui::Selectable( category.sName.c_str(), selected == category.sName ) )
		{
			selected = category.sName;
		}
	}
}
void ProjectSettingsDisplay::DrawSettingsPanel( const SettingCategory& category, std::string& selected )
{
	if ( category.sName == selected )
	{
		for ( const auto& item : category.items )
		{
			item.drawFunc();
			ImGui::Separator();
		}
	}

	for ( const auto& sub : category.subCategories )
	{
		DrawSettingsPanel( sub, selected );
	}
}

auto vector_getter = []( void* vec, int idx, const char** out_text ) {
	auto& vector = *static_cast<std::vector<std::string>*>( vec );
	if ( idx < 0 || idx >= static_cast<int>( vector.size() ) )
	{
		return false;
	}
	*out_text = vector[ idx ].c_str();
	return true;
};

void ProjectSettingsDisplay::CreateProjectSettings()
{
	auto& coreGlobals = CORE_GLOBALS();
	auto& mainRegistry = MAIN_REGISTRY();
	auto& pProjectInfo = mainRegistry.GetContext<Scion::Core::ProjectInfoPtr>();
	SCION_ASSERT( pProjectInfo && "Project Info was not setup correctly." );

	SettingCategory projectSettings{ .sName = "Project" };
	projectSettings.subCategories.emplace_back( CreateGeneralSettings( coreGlobals, *pProjectInfo, mainRegistry ) );
	projectSettings.subCategories.emplace_back( CreatePhysicsSettings( coreGlobals, *pProjectInfo, mainRegistry ) );
	projectSettings.subCategories.emplace_back( CreateGraphicsSettings( coreGlobals, *pProjectInfo, mainRegistry ) );
	projectSettings.subCategories.emplace_back( CreateAudioSettings( coreGlobals, *pProjectInfo, mainRegistry ) );

	m_SettingsCategory = projectSettings;
}

// clang-format off
ProjectSettingsDisplay::SettingCategory ProjectSettingsDisplay::CreateGeneralSettings(
	Scion::Core::CoreEngineData& coreGlobals, Scion::Core::ProjectInfo& projectInfo,
	Scion::Core::ECS::MainRegistry& mainRegistry )
{
	return SettingCategory{
		.sName = "General",
		.items = {
			{ "Project Name",
				[ & ]() {
					std::string sGameName = projectInfo.GetProjectName();
					ImGui::InlineLabel( "Project Name" );
					if ( ImGui::InputText( "##project_name", &sGameName ) )
					{
						projectInfo.SetProjectName( sGameName );
					}
				}
			},
			{ "Version",
				[ & ]() {
					std::string sVersion{ projectInfo.GetProjectVersion() };
					ImGui::InlineLabel( "Version" );
					if ( ImGui::InputText( "##version", &sVersion ) )
					{
						projectInfo.SetProjectVersion( sVersion );
					}
				}
			},
			{ "Copyright Notice",
				[ & ]() {
					std::string sCopyright{ projectInfo.GetCopyRightNotice() };
					ImGui::InlineLabel( "Copyright Notice" );
					if ( ImGui::InputText( "##copyright_notice", &sCopyright ) )
					{
						projectInfo.SetCopyRightNotice( sCopyright );
					}
				}
			},
			{ "Description",
				[ & ]() {
					std::string sDescription{ projectInfo.GetProjectDescription() };
					ImGui::InlineLabel( "Description" );
					if ( ImGui::InputText( "##description", &sDescription ) )
					{
						projectInfo.SetProjectDescription( sDescription );
					}
				}
			},
			{ "Icon",
				[ & ]() {
					static bool bHasError{ false };
					static std::string sIconPath{
						projectInfo.GetFileIconPath() ? projectInfo.GetFileIconPath()->string() : "" };
					ImGui::InlineLabel( "Icon:" );
					ImGui::PushItemWidth( 256.f );
					ImGui::InputText( "##icon", &sIconPath, ImGuiInputTextFlags_ReadOnly );
					ImGui::SameLine();
					if ( ImGui::Button( "..."
										"##iconpath" ) )
					{
						auto optContentPath =
							projectInfo.TryGetFolderPath( Scion::Core::EProjectFolderType::Content );
						SCION_ASSERT( optContentPath && "Content folder not set correctly in project info." );
						FileDialog fd{};

						std::string sOpenIcoPath = fd.OpenFileDialog(
							"Open Icon File", optContentPath->string(), { "*.ico" }, "Ico Files (*.ico)" );

						if ( !sOpenIcoPath.empty() )
						{
							if ( sOpenIcoPath.starts_with( optContentPath->string() ) )
							{
								if ( sOpenIcoPath != sIconPath && fs::exists( fs::path{ sOpenIcoPath } ) )
								{
									projectInfo.SetFileIconPath( fs::path{ sOpenIcoPath } );
									sIconPath = sOpenIcoPath;
									bHasError = false;
								}
							}
							else
							{
								SCION_ERROR( "Failed to change icon. Path [{}] is invalid. Icons must be in the "
											"project content.",
											sOpenIcoPath );

								bHasError = true;
							}
						}
						else
						{
							bHasError = false;
						}
					}

					if ( auto* pIconTexture = projectInfo.GetIconTexturePtr() )
					{
						ImGui::SameLine( 0.f, 32.f );
						ImGui::Image( (ImTextureID)(intptr_t)pIconTexture->GetID(), ImVec2{ 64.f, 64.f } );
					}

					if ( bHasError )
					{
						ImGui::TextColored(
							ImVec4{ 1.f, 0.f, 0.f, 1.f },
							"Invalid path. The game icon must be in the content folder for the project." );
					}
				}
			},
			{ "GameType",
				[ & ]() {
					std::string sGameType{ coreGlobals.GetGameTypeStr( coreGlobals.GetGameType() ) };
					ImGui::InlineLabel( ICON_FA_GAMEPAD " Game Type:" );
					ImGui::ItemToolTip( "The type of game this is going to be." );
					if ( ImGui::BeginCombo( "##game_type", sGameType.c_str() ) )
					{
						for ( const auto& [ eType, sTypeStr ] : coreGlobals.GetGameTypesMap() )
						{
							if ( ImGui::Selectable( sTypeStr.c_str(), sTypeStr == sGameType ) )
							{
								sGameType = sTypeStr;
								coreGlobals.SetGameType( eType );
							}
						}

						ImGui::EndCombo();
					}
				}
			},
			{ "Default Scene",
				[ & ]() {
					std::string sDefaultScene{ projectInfo.GetDefaultScene() };
					ImGui::InlineLabel( ICON_FA_IMAGE " Default Scene" );
					ImGui::ItemToolTip( "The default scene to be loaded when the project is loaded." );
					if ( ImGui::BeginCombo( "##default_scene", sDefaultScene.c_str() ) )
					{
						for ( const auto& sSceneName : SCENE_MANAGER().GetSceneNames())
						{
							if ( ImGui::Selectable( sSceneName.c_str(), sSceneName == sDefaultScene ) )
							{
								sDefaultScene = sSceneName;
								projectInfo.SetDefaultScene(sDefaultScene);
							}
						}

						ImGui::EndCombo();
					}
				}
			}
		}
	};
}

ProjectSettingsDisplay::SettingCategory ProjectSettingsDisplay::CreatePhysicsSettings(
	Scion::Core::CoreEngineData& coreGlobals, Scion::Core::ProjectInfo& projectInfo,
	Scion::Core::ECS::MainRegistry& mainRegistry )
{
	return SettingCategory{
		.sName = "Physics",
		.items = {
			{ "Enabled",
				[ & ]() {
					static bool bEnabled{ coreGlobals.IsPhysicsEnabled() };
					ImGui::InlineLabel( "Enable" );
					if ( ImGui::Checkbox( "##physicsEnable", &bEnabled ) )
					{
						bEnabled ? coreGlobals.EnablePhysics() : coreGlobals.DisablePhysics();
					}
				}
			},
			{ "Iterations",
				[ & ]() {
					static int velocity{ coreGlobals.GetVelocityIterations() };
					static int position{ coreGlobals.GetPositionIterations() };
					ImGui::InlineLabel( "Position" );
					if ( ImGui::InputInt( "##positionItr", &position ) )
					{
						coreGlobals.SetPositionIterations( position );
					}
					ImGui::InlineLabel( "Velocity" );
					if ( ImGui::InputInt( "##velocityItr", &velocity ) )
					{
						coreGlobals.SetVelocityIterations( velocity );
					}
				}
			},
			{ "Gravity",
				[ & ]() {
					static float gravity{ coreGlobals.GetGravity() };
					ImGui::InlineLabel( "Gravity" );
					if ( ImGui::InputFloat( "##gravity", &gravity ) )
					{
						coreGlobals.SetGravity( gravity );
					}
				}
			},
			{ "Collision Categories",
				[ & ]() {
					static float gravity{ coreGlobals.GetGravity() };
					static int currentIndex{ 0 };
					auto filters = GetFilterStrings();
					ImGui::InlineLabel( "Collision Categories" );
					if ( ImGui::ListBox( "##collisionCategories",
										&currentIndex,
										vector_getter,
										static_cast<void*>( &filters ),
										static_cast<int>( filters.size() ) ) )
					{
					}
				}
			},
		}
	};
}

ProjectSettingsDisplay::SettingCategory ProjectSettingsDisplay::CreateGraphicsSettings(
	Scion::Core::CoreEngineData& coreGlobals, Scion::Core::ProjectInfo& projectInfo,
	Scion::Core::ECS::MainRegistry& mainRegistry )
{
	return SettingCategory{ "Graphics",
							{
								{ "Resolution",
								  [&]() {
									  ImGui::SeparatorText("Graphics Settings");

									  int width{ coreGlobals.WindowWidth() };
									  int height{ coreGlobals.WindowHeight() };

									  ImGui::InlineLabel("Width");
									  if (ImGui::InputInt("##windowwidth", &width))
									  {
										  coreGlobals.SetWindowWidth(width);
									  }
									  ImGui::InlineLabel("Height");
									  if (ImGui::InputInt("##windowheight", &height))
									  {
										  coreGlobals.SetWindowHeight(height);
									  }
								  } },
							} };
}

ProjectSettingsDisplay::SettingCategory ProjectSettingsDisplay::CreateAudioSettings(
	Scion::Core::CoreEngineData& coreGlobals, Scion::Core::ProjectInfo& projectInfo,
	Scion::Core::ECS::MainRegistry& mainRegistry )
{
	return SettingCategory{
		"Audio",
		{},
		{
			{
				"Global Overrides",
				{
					{
						"Overrides",
						[&]
						{
							ImGui::SeparatorText("Global Sound Overrides");
							ImGui::InlineLabel("Enable");
							ImGui::Checkbox("##enable_override", &projectInfo.GetAudioConfig().bGlobalOverrideEnabled);
							ImGui::InlineLabel("Volume");
							if (ImGui::InputInt("##override_volume", &projectInfo.GetAudioConfig().globalVolumeOverride))
							{
								projectInfo.GetAudioConfig().globalVolumeOverride =
									std::clamp(projectInfo.GetAudioConfig().globalVolumeOverride, 0, 100);
							}
						}
					}
				},
				{} 
			},
			{
				"Music Overrides",
				{
					{
						"Music Overrides",
						[&]
						{
							ImGui::SeparatorText("Music Channel Overrides");
							ImGui::InlineLabel("Enable");
							ImGui::ItemToolTip("Enables music channel override. If enabled, the volume set here will override other settings.");
							ImGui::Checkbox("##enable_music_override", &projectInfo.GetAudioConfig().bMusicVolumeOverrideEnabled);
							ImGui::InlineLabel("Volume");
							ImGui::ItemToolTip("Music channel volume override.");
							if (ImGui::InputInt("##override_music_volume", &projectInfo.GetAudioConfig().musicVolumeOverride))
							{
								projectInfo.GetAudioConfig().musicVolumeOverride=
									std::clamp(projectInfo.GetAudioConfig().musicVolumeOverride, 0, 100);
							}
						}
					}
				},
				{}
			},
			{
				"Sound fx Overrides",
				{
					{
						"SoundFx Overrides",
						[&]
						{
							ImGui::SeparatorText("Sound Fx Channel Settings");
							int allocatedChannels{ projectInfo.GetAudioConfig().GetAllocatedChannelCount() };
							ImGui::InlineLabel("Channels");
							ImGui::ItemToolTip("The number of allocated channels. Min 8 channels. Max 64 channels.");
							if (ImGui::InputInt("##channels_allocated", &allocatedChannels))
							{
								int newChannelCount{allocatedChannels - projectInfo.GetAudioConfig().GetAllocatedChannelCount() };
								if (newChannelCount != 0)
								{
									if (!projectInfo.GetAudioConfig().UpdateSoundChannels(newChannelCount))
									{
										SCION_ERROR("Failed to update sound channels.");
									}
								}
							}

							if (ImGui::BeginTable("ChannelTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
							{
								ImGui::TableSetupColumn("Channel");
								ImGui::TableSetupColumn("Enabled");
								ImGui::TableSetupColumn("Volume");
								ImGui::TableHeadersRow();

								for (const auto& [channelId, state] : projectInfo.GetAudioConfig().GetSoundChannelMap())
								{
									bool bEnabled{ state.first };
									int volume{ state.second };

									ImGui::TableNextRow();

									ImGui::TableSetColumnIndex(0);
									ImGui::Text("%d", channelId);

									ImGui::TableSetColumnIndex(1);
									if (ImGui::Checkbox(("##enabled_" + std::to_string(channelId)).c_str(), &bEnabled))
									{
										projectInfo.GetAudioConfig().EnableChannelOverride(channelId, bEnabled);
									}

									ImGui::TableSetColumnIndex(2);
									if (ImGui::SliderInt(("##volume_" + std::to_string(channelId)).c_str(), &volume, 0, 100))
									{
										projectInfo.GetAudioConfig().SetChannelVolume(channelId, volume);
									}
								}

								ImGui::EndTable();
							}
						}
					}
				},
				{}
			}
		}
	};
}

// clang-format on
} // namespace Scion::Editor
