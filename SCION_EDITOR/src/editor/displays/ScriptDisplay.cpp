#include "ScriptDisplay.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/CoreUtilities/ProjectInfo.h"

#include "ScionUtilities/HelperUtilities.h"
#include "ScionUtilities/ScionUtilities.h"

#include "ScionFilesystem/Serializers/LuaSerializer.h"
#include "ScionFilesystem/Utilities/DirectoryWatcher.h"

#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/utilities/EditorState.h"
#include "Logger/Logger.h"

#include <imgui.h>

namespace fs = std::filesystem;

using namespace SCION_CORE;
using namespace SCION_FILESYSTEM;

namespace SCION_EDITOR
{

ScriptDisplay::ScriptDisplay()
	: m_sScriptsDirectory{}
	, m_Selected{ -1 }
	, m_bScriptsChanged{ false }
	, m_bListScripts{ false }
	, m_pDirWatcher{ nullptr }
	, m_bFilesChanged{ false }
{
	auto& pProjectInfo = MAIN_REGISTRY().GetContext<ProjectInfoPtr>();
	auto optScriptsPath = pProjectInfo->TryGetFolderPath( EProjectFolderType::Scripts );
	SCION_ASSERT( optScriptsPath && "Scipts Directory was not set correctly." );
	SCION_ASSERT( fs::exists( *optScriptsPath ) && "Scripts directory must exist." );

	m_sScriptsDirectory = optScriptsPath->string();

	auto optScriptListPath = pProjectInfo->GetScriptListPath();
	SCION_ASSERT( optScriptListPath && "Script list path not set correctly in project info." );

	if ( !fs::exists( *optScriptListPath ) )
	{
		std::ofstream file{ optScriptListPath->string() };
		file.close();

		if ( !fs::exists( *optScriptListPath ) )
		{
			SCION_ASSERT( false && "Failed to create script list file." );

			SCION_ERROR( "Failed to create script list file at path: [{}].",
						 optScriptListPath->parent_path().string() );
			return;
		}
	}

	GenerateScriptList();

	m_pDirWatcher = std::make_unique<SCION_FILESYSTEM::DirectoryWatcher>(
		fs::path{ m_sScriptsDirectory },
		[ this ]( const fs::path& file, bool bModified ) { OnFileChanged( file, bModified ); } );
}

ScriptDisplay::~ScriptDisplay() = default;

void ScriptDisplay::Draw()
{
	if ( auto& pEditorState = MAIN_REGISTRY().GetContext<EditorStatePtr>() )
	{
		if ( !pEditorState->IsDisplayOpen( EDisplay::ScriptListView ) )
		{
			return;
		}
	}

	if ( !ImGui::Begin( ICON_FA_CODE " Script List" ) )
	{
		ImGui::End();
		return;
	}

	ImGui::SeparatorText( "Script List" );

	ImGui::Text( "Drag to reorder scripts:" );
	ImGui::Separator();

	ImGui::Columns( 2, nullptr, false );

	ImGui::BeginChild( "ListBox", ImVec2{ 0.f, 300.f }, true );
	for ( int i = 0; i < static_cast<int>( m_ScriptList.size() ); i++ )
	{
		const auto& script = m_ScriptList[ i ];
		if ( ImGui::Selectable( script.c_str(), m_Selected == i ) )
		{
			m_Selected = i;
		}

		if ( ImGui::IsItemActive() && !ImGui::IsItemHovered() )
		{
			int iNext = i + ( ImGui::GetMouseDragDelta( 0 ).y < 0.f ? -1 : 1 );
			if ( iNext >= 0 && iNext < m_ScriptList.size() )
			{
				std::swap( m_ScriptList.at( i ), m_ScriptList.at( iNext ) );
				ImGui::ResetMouseDragDelta();
				m_bScriptsChanged = true;
			}
		}
	}

	ImGui::EndChild();

	ImGui::NextColumn();

	ImGui::BeginChild( "Controls", ImVec2{ 0.f, 300.f }, false, ImGuiWindowFlags_NoBackground );

	float buttonHeight = ImGui::GetFrameHeight();
	float totalHeight = buttonHeight * 2 + ImGui::GetStyle().ItemSpacing.y;

	float available = ImGui::GetContentRegionAvail().y;
	float padding = ( available - totalHeight ) * 0.5f;

	if ( padding > 0.f )
	{
		ImGui::Dummy( ImVec2{ 0.f, padding } );
	}

	if ( ImGui::ArrowButton( "##Up", ImGuiDir_Up ) )
	{
		if ( m_Selected >= 0 && !m_ScriptList.empty() )
		{
			int newIndex = ( m_Selected - 1 + m_ScriptList.size() ) % m_ScriptList.size();
			std::swap( m_ScriptList[ m_Selected ], m_ScriptList[ newIndex ] );
			m_Selected = newIndex;
			m_bScriptsChanged = true;
		}
	}

	if ( ImGui::ArrowButton( "##Down", ImGuiDir_Down ) )
	{
		if ( m_Selected >= 0 && !m_ScriptList.empty() )
		{
			int newIndex = ( m_Selected + 1 ) % m_ScriptList.size();
			std::swap( m_ScriptList[ m_Selected ], m_ScriptList[ newIndex ] );
			m_Selected = newIndex;
			m_bScriptsChanged = true;
		}
	}

	ImGui::EndChild();

	ImGui::Columns( 1 );

	if ( m_bScriptsChanged )
	{
		if ( ImGui::Button( "Regenerate Script List" ) )
		{
			WriteScriptListToFile();
		}
	}

	ImGui::End();
}

void ScriptDisplay::Update()
{
	if ( m_bFilesChanged.exchange( false, std::memory_order_acquire ) )
	{
		m_bListScripts = true;
	}

	if ( m_bListScripts )
	{
		m_Scripts.clear();
		for ( const auto& dirEntry : fs::recursive_directory_iterator( fs::path{ m_sScriptsDirectory } ) )
		{
			if ( fs::is_directory( dirEntry ) || dirEntry.path().extension() != ".lua" ||
				 dirEntry.path().filename().string() == "main.lua" ||
				 dirEntry.path().filename().string() == "script_list.lua" )
			{
				continue;
			}

			auto foundScript = GET_SUBSTRING( dirEntry.path().relative_path().string(), "scripts" );
			if ( !foundScript.empty() )
			{
				m_Scripts.push_back( std::string{ foundScript } );
			}
		}

		std::unordered_set<std::string> lookupSet( m_Scripts.begin(), m_Scripts.end() );

		auto removeRange = std::ranges::remove_if(
			m_ScriptList, [ &lookupSet ]( const std::string& item ) { return !lookupSet.contains( item ); } );

		if ( removeRange.begin() != removeRange.end() )
		{
			m_ScriptList.erase( removeRange.begin(), removeRange.end() );
			m_bScriptsChanged = true;
		}

		for ( const auto& script : m_Scripts )
		{
			if ( std::ranges::find( m_ScriptList, script ) == m_ScriptList.end() )
			{
				m_ScriptList.push_back( script );
				m_bScriptsChanged = true;
			}
		}

		m_bListScripts = false;
	}
}

void ScriptDisplay::GenerateScriptList()
{
	if ( m_ScriptList.empty() )
	{
		auto optScriptListPath = MAIN_REGISTRY().GetContext<ProjectInfoPtr>()->GetScriptListPath();
		SCION_ASSERT( optScriptListPath && "ScriptList Path not set correctly in project info." );

		if (!optScriptListPath)
		{
			SCION_ERROR( "Failed to load script list. Not set correctly in project info." );
			return;
		}
		if ( fs::exists( *optScriptListPath ) )
		{
			sol::state lua{};
			auto result = lua.safe_script_file( optScriptListPath->string() );
			if ( !result.valid() )
			{
				sol::error err = result;
				SCION_ERROR( "Failed to load script list. {}", err.what() );
				return;
			}

			sol::optional<sol::table> scriptList = lua[ "ScriptList" ];
			if ( !scriptList )
			{
				SCION_ERROR( "Failed to load script list. Missing \"ScriptList\" table." );
				return;
			}

			std::string sPath{ m_sScriptsDirectory.substr( 0, m_sScriptsDirectory.find( "scripts" ) ) };
			for ( const auto& [ _, script ] : *scriptList )
			{
				std::string newScript{ script.as<std::string>() };
				if ( fs::exists( fs::path{ sPath + newScript } ) )
				{
					m_ScriptList.push_back( newScript );
				}
			}
		}
	}
}

void ScriptDisplay::WriteScriptListToFile()
{
	auto& pProjectInfo = MAIN_REGISTRY().GetContext<SCION_CORE::ProjectInfoPtr>();
	SCION_ASSERT( pProjectInfo && "Project info must exist." );
	auto optScriptListPath = pProjectInfo->GetScriptListPath();
	SCION_ASSERT( optScriptListPath && "Script list path not setup correctly in project info." );

	if ( !fs::exists( *optScriptListPath ) )
	{
		SCION_ERROR( "Failed to write script list. File [{}] does not exist.", optScriptListPath->string() );
		return;
	}

	std::unique_ptr<LuaSerializer> pSerializer{ nullptr };
	try
	{
		pSerializer = std::make_unique<LuaSerializer>( optScriptListPath->string() );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to write script list. {}", ex.what() );
		return;
	}

	pSerializer->AddComment( "Script List File" )
		.AddComment( "WARNING - THIS FILE IS EDITOR GENERATED!" )
		.AddComment( "DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING." )
		.AddComment( "\n" );

	pSerializer->StartNewTable( "ScriptList" );
	for ( const auto& script : m_ScriptList )
	{
		pSerializer->AddValue( script, true, false, false, true );
	}

	pSerializer->EndTable().FinishStream();
	m_bScriptsChanged = false;
}

void ScriptDisplay::OnFileChanged( const std::filesystem::path& path, bool bModified )
{
	m_bFilesChanged.store( true, std::memory_order_relaxed );
}

} // namespace SCION_EDITOR
