#include "PackageDisplay.h"
#include "Core/CoreUtilities/ProjectInfo.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/ECS/MainRegistry.h"
#include "ScionUtilities/HelperUtilities.h"
#include "ScionUtilities/ThreadPool.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/loaders/ProjectLoader.h"
#include "editor/packaging/Packager.h"
#include "editor/scene/SceneManager.h"
#include "ScionFilesystem/Dialogs/FileDialog.h"
#include "Logger/Logger.h"

#include <imgui.h>
#include <imgui_stdlib.h>

namespace fs = std::filesystem;
using namespace SCION_FILESYSTEM;

namespace SCION_EDITOR
{
PackageGameDisplay::PackageGameDisplay()
	: m_pGameConfig{ std::make_unique<SCION_CORE::GameConfig>() }
	, m_pPackager{ nullptr }
	, m_sDestinationPath{}
	, m_sScriptListPath{}
	, m_sFileIconPath{}
	, m_bResizable{ false }
	, m_bBorderless{ false }
	, m_bFullScreen{ false }
	, m_bTitlebar{ false }
	, m_bScriptListExists{ false }
	, m_bPackageHasErrors{ false }
{
	const auto& pProjectInfo = MAIN_REGISTRY().GetContext<SCION_CORE::ProjectInfoPtr>();
	auto optScriptListPath = pProjectInfo->GetScriptListPath();
	SCION_ASSERT( optScriptListPath && "Script List path not set correctly in project info." );

	m_sScriptListPath = optScriptListPath->string();
	m_bScriptListExists = fs::exists( *optScriptListPath );
}

PackageGameDisplay::~PackageGameDisplay() = default;

void PackageGameDisplay::Update()
{
	if ( !m_pPackager )
		return;
	if ( m_pPackager->Completed() )
	{
		m_pPackager.reset( nullptr );
		return;
	}

	if ( m_pPackager->HasError() )
	{
		m_bPackageHasErrors = true;
		m_pPackager.reset( nullptr );
	}
}

void PackageGameDisplay::Draw()
{
	if ( !ImGui::Begin( ICON_FA_ARCHIVE " Package Game" ) )
	{
		ImGui::End();
		return;
	}

	auto& pProjectInfo = MAIN_REGISTRY().GetContext<SCION_CORE::ProjectInfoPtr>();

	ImGui::SeparatorText( "Package and Export Game" );
	ImGui::NewLine();

	if ( ImGui::BeginChild( "Game Packager" ) )
	{
		ImGui::SeparatorText( "File Information" );
		ImGui::PushItemWidth( 256.f );
		ImGui::InlineLabel( "Game Title" );
		std::string sProjectName{ pProjectInfo->GetProjectName() };
		ImGui::InputTextReadOnly( "##gameTitle", &sProjectName );

		static bool bDestinationError{ false };
		ImGui::InlineLabel( "Destination" );
		ImGui::InputTextReadOnly( "##destination", &m_sDestinationPath );
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if ( ImGui::Button( "..."
							"##dest" ) )
		{
			SCION_FILESYSTEM::FileDialog fd{};
			const auto sFilepath = fd.SelectFolderDialog( "Choose Destination Folder", BASE_PATH );
			if ( !sFilepath.empty() )
			{
				if ( !IsReservedPathOrFile( fs::path{ sFilepath } ) )
				{
					m_sDestinationPath = sFilepath;
					bDestinationError = false;
				}
				else
				{
					SCION_ERROR( "Failed to set destination. "
								 "Destination [{}] is a reserved path. "
								 "Please select a different path.",
								 sFilepath );

					bDestinationError = true;
				}
			}
			else
			{
				bDestinationError = false;
			}
		}

		if ( bDestinationError )
		{
			ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f },
								"Invalid Destination. Destinations cannot be reserved paths." );
		}

		ImGui::InlineLabel( "Package Assets" );
		ImGui::ItemToolTip( "Convert assets into luac files and add them to zip archive." );
		ImGui::Checkbox( "##packageassets", &m_pGameConfig->bPackageAssets );
		ImGui::AddSpaces( 2 );
		ImGui::Separator();
		ImGui::AddSpaces( 3 );

		ImGui::SeparatorText( "Default Window Parameters" );
		ImGui::AddSpaces( 2 );
		ImGui::InlineLabel( "Window Size" );
		ImGui::PushItemWidth( 128.f );
		ImGui::InputInt( "##windowWidth", &m_pGameConfig->windowWidth );
		ImGui::SameLine();
		ImGui::InputInt( "##windowHeight", &m_pGameConfig->windowHeight );
		ImGui::PopItemWidth();
		ImGui::Separator();
		ImGui::AddSpaces( 2 );
		ImGui::Text( "Window Flags" );
		ImGui::Separator();

		ImGui::InlineLabel( "Resizable" );
		if ( ImGui::Checkbox( "##resizable", &m_bResizable ) )
		{
			if ( m_bResizable )
			{
				m_bFullScreen = false;
			}
		}

		ImGui::InlineLabel( "Fullscreen" );
		if ( ImGui::Checkbox( "##fullscreen", &m_bFullScreen ) )
		{
			if ( m_bFullScreen )
			{
				m_bResizable = false;
				m_bTitlebar = false;
			}
		}

		ImGui::InlineLabel( "Titlebar" );
		if ( ImGui::Checkbox( "##titlebar", &m_bTitlebar ) )
		{
			if ( m_bTitlebar )
			{
				m_bFullScreen = false;
				m_bBorderless = false;
			}
		}

		ImGui::InlineLabel( "Borderless" );
		if ( ImGui::Checkbox( "##borderless", &m_bBorderless ) )
		{
			if ( m_bBorderless )
			{
				m_bTitlebar = false;
			}
		}

		ImGui::Separator();
		ImGui::AddSpaces( 3 );
		ImGui::SeparatorText( "Default Camera Parameters" );
		ImGui::AddSpaces( 3 );

		ImGui::InlineLabel( "Camera Size" );
		ImGui::PushItemWidth( 128.f );
		ImGui::InputInt( "##cameraWidth", &m_pGameConfig->cameraWidth );
		ImGui::SameLine();
		ImGui::InputInt( "##cameraHeight", &m_pGameConfig->cameraHeight );

		ImGui::InlineLabel( "Camera Scale" );
		ImGui::InputFloat( "##camerascale", &m_pGameConfig->cameraScale );
		ImGui::PopItemWidth();
		ImGui::AddSpaces( 3 );

		ImGui::SeparatorText( "Startup Options" );

		ImGui::PushItemWidth( 256.f );

		ImGui::InlineLabel( "Startup Scene" );
		if ( ImGui::BeginCombo( "##start_up_scenes", m_pGameConfig->sStartupScene.c_str() ) )
		{
			for ( const auto& sSceneName : SCENE_MANAGER().GetSceneNames() )
			{
				if ( ImGui::Selectable( sSceneName.c_str(), sSceneName == m_pGameConfig->sStartupScene ) )
				{
					m_pGameConfig->sStartupScene = sSceneName;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();

		ImGui::EndChild();
	}

	ImGui::Separator();

	ImGui::AddSpaces( 3 );

	if ( CanPackageGame() )
	{
		if ( m_pPackager && !m_pPackager->Completed() )
		{
			ImGui::LoadingSpinner( "##packaging", 10.f, 3.f, IM_COL32( 32, 175, 32, 255 ) );
			ImGui::SameLine( 0.f, 16.f );
			const auto packageProgress = m_pPackager->GetProgress();
			if ( auto pFont = ImGui::GetFont( "roboto-bold-24" ) )
			{
				ImGui::PushFont( pFont );
				ImGui::TextColored(
					m_bPackageHasErrors ? ImVec4{ 1.f, 0.f, 0.f, 1.f } : ImVec4{ 0.f, 1.f, 0.f, 1.f },
					fmt::format( "{}%% - {}", packageProgress.percent, packageProgress.sMessage ).c_str() );
				ImGui::PopFont();
			}
		}
		else if ( ImGui::Button( "Package Game" ) )
		{
			// We want to ensure we are packaging the most current data.
			// Save all files, before packaging.
			auto& pProjectInfo = MAIN_REGISTRY().GetContext<SCION_CORE::ProjectInfoPtr>();
			SCION_ASSERT( pProjectInfo && "Project Info must exist!" );
			// Save entire project
			ProjectLoader pl{};
			if ( !pl.SaveLoadedProject( *pProjectInfo ) )
			{
				auto optProjectFilePath = pProjectInfo->GetProjectFilePath();
				SCION_ASSERT( optProjectFilePath && "Project file path not set correctly in project info." );

				SCION_ERROR( "Failed to save project [{}] at file [{}].",
							 pProjectInfo->GetProjectName(),
							 optProjectFilePath->string() );

				return;
			}

			const auto& coreGlobals = CORE_GLOBALS();

			// Set Physics data
			m_pGameConfig->bPhysicsEnabled = coreGlobals.IsPhysicsEnabled();
			m_pGameConfig->positionIterations = coreGlobals.GetPositionIterations();
			m_pGameConfig->velocityIterations = coreGlobals.GetVelocityIterations();
			m_pGameConfig->gravity = coreGlobals.GetGravity();
			m_pGameConfig->sGameName = pProjectInfo->GetProjectName();

			// Set window flags
			uint32_t flags{ 0 };

			if ( m_bBorderless && !m_bTitlebar )
				flags |= SDL_WINDOW_BORDERLESS;
			if ( m_bFullScreen )
				flags |= SDL_WINDOW_FULLSCREEN;
			if ( m_bResizable )
				flags |= SDL_WINDOW_RESIZABLE;

			m_pGameConfig->windowFlags = flags;

			std::string sFullDestination =
				fmt::format( "{}{}{}", m_sDestinationPath, PATH_SEPARATOR, m_pGameConfig->sGameName );

			auto pPackageData = std::make_unique<PackageData>();
			pPackageData->pProjectInfo = std::make_unique<SCION_CORE::ProjectInfo>( *pProjectInfo );
			pPackageData->pGameConfig = std::make_unique<SCION_CORE::GameConfig>( *m_pGameConfig );
			pPackageData->sTempDataPath = fs::path{ pProjectInfo->GetProjectPath() / "tempData" }.string();
			pPackageData->sFinalDestination = sFullDestination;
			pPackageData->sAssetFilepath = pPackageData->sTempDataPath + PATH_SEPARATOR + "assetDefs.lua";

			auto& pThreadPool = MAIN_REGISTRY().GetContext<SharedThreadPool>();
			SCION_ASSERT( pThreadPool && "Thread pool must exist and be valid." );

			m_pPackager = std::make_unique<Packager>( std::move( pPackageData ), pThreadPool );

			ImGui::End();

			return;
		}
	}
	else if ( !m_bScriptListExists )
	{
		ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f }, "Unable to package game. Script List does not exist." );
	}

	ImGui::End();
}

bool PackageGameDisplay::CanPackageGame() const
{
	return m_bScriptListExists && !m_sDestinationPath.empty() && !m_pGameConfig->sStartupScene.empty() &&
		   fs::exists( fs::path{ m_sDestinationPath } ) && !m_sDestinationPath.empty();
}

} // namespace SCION_EDITOR
