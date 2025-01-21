#include "ContentDisplay.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "ScionUtilities/ScionUtilities.h"
#include "ScionFilesystem/Dialogs/FileDialog.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "Logger/Logger.h"

#include "editor/events/EditorEventTypes.h"
#include "editor/utilities/SaveProject.h"
#include "Core/Events/EventDispatcher.h"

#include <imgui.h>

using namespace SCION_EDITOR::Events;

namespace fs = std::filesystem;

namespace SCION_EDITOR
{
ContentDisplay::ContentDisplay()
	: m_pFileDispatcher{ std::make_unique<SCION_CORE::Events::EventDispatcher>() }
	, m_CurrentDir{ MAIN_REGISTRY().GetContext<std::shared_ptr<SaveProject>>()->sProjectPath + "content" }
	, m_sFilepathToAction{ "" }
	, m_Selected{ -1 }
	, m_eFileAction{ Events::EFileAction::NoAction }
	, m_bItemCut{ false }
	, m_bWindowHovered{ false }
{
	ADD_EVENT_HANDLER( FileEvent, &ContentDisplay::HandleFileEvent, *this );
	m_pFileDispatcher->AddHandler<FileEvent, &ContentDisplay::HandleFileEvent>( *this );
}

ContentDisplay::~ContentDisplay()
{
}

void ContentDisplay::Update()
{
	m_pFileDispatcher->UpdateAll();
}

void ContentDisplay::Draw()
{
	if ( !ImGui::Begin( "Content Browser" ) )
	{
		ImGui::End();
		return;
	}

	const auto& windowWidth = ImGui::GetWindowWidth();
	int numCols = windowWidth / 128;

	if ( numCols == 0 )
	{
		SCION_ERROR( "NumCols is zero!" );
		ImGui::End();
		return;
	}

	DrawToolbar();

	auto size = static_cast<int>( std::distance( fs::directory_iterator( m_CurrentDir ), fs::directory_iterator{} ) );
	int numRows = size / numCols < 1 ? 1 : ( size / numCols ) + 1;
	auto itr = fs::directory_iterator( m_CurrentDir );

	if ( ImGui::BeginTable( "Content", numCols, IMGUI_NORMAL_TABLE_FLAGS ) )
	{
		m_bWindowHovered = ImGui::IsWindowHovered();
		static ImGuiID popID = 0;

		for ( int i = 0; i < numRows; i++ )
		{
			ImGui::TableNextRow();
			for ( int j = 0; j < numCols; j++ )
			{
				if ( itr == fs::directory_iterator{} )
					break;

				const auto& path = itr->path();

				auto relativePath = fs::relative( path, m_CurrentDir );
				auto filenameStr = relativePath.filename().string();

				ImGui::TableSetColumnIndex( j );
				ImGui::PushID( j + j + i );
				int id = j + j + i;

				if ( m_Selected == id )
				{
					ImGui::TableSetBgColor( ImGuiTableBgTarget_CellBg,
											ImGui::GetColorU32( ImVec4{ 0.f, 0.9f, 0.f, 0.3f } ) );
				}

				const auto* icon = GetIconTexture( path.string() );
				static bool bItemPop{ false };

				std::string contentBtn = "##content_" + std::to_string( id );
				if ( itr->is_directory() )
				{
					// Change to the next Directory
					ImGui::ImageButton( contentBtn.c_str(), ( ImTextureID )( intptr_t ) icon->GetID(), ImVec2{ 80.f, 80.f } );
					if ( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( 0 ) )
					{
						m_CurrentDir /= path.filename();
						m_Selected = -1;
					}
					else if ( ImGui::IsItemHovered() && ImGui::IsMouseClicked( 0 ) )
					{
						m_Selected = id;
					}
					else if ( !ImGui::IsItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked( 0 ) )
					{
						m_Selected = -1;
					}
				}
				else
				{
					ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, { 0.0f, 0.0f } );
					ImGui::ImageButton( contentBtn.c_str(), ( ImTextureID )( intptr_t ) icon->GetID(), ImVec2{ 80.f, 80.f } );
					ImGui::PopStyleVar( 1 );

					if ( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( 0 ) )
					{
						// TODO: Open new process to run the file.
						// Needs to be a cross platform solution
					}
					else if ( ImGui::IsItemHovered() && ImGui::IsMouseClicked( 0 ) )
					{
						m_Selected = id;
					}
				}

				if ( ImGui::BeginPopupContextItem() )
				{
					popID = ImGui::GetItemID();
					if ( m_bItemCut )
					{
						ImGui::BeginDisabled();
						ImGui::Selectable( "Cut" );
						ImGui::EndDisabled();
					}
					else
					{
						if ( ImGui::Selectable( "Cut" ) )
						{
							m_sFilepathToAction = path.string();
							m_bItemCut = true;
						}

						if ( ImGui::Selectable( "Delete" ) )
						{
							if ( m_Selected == id )
							{
								m_sFilepathToAction = path.string();
								m_eFileAction = Events::EFileAction::Delete;
							}
						}
					}

					bItemPop = true;
					ImGui::EndPopup();
				}

				ImGui::SetNextItemWidth( 80.f );
				ImGui::TextWrapped( filenameStr.c_str() );

				if ( !ImGui::IsPopupOpen( "", ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel ) && bItemPop )
				{
					popID = 0;
					bItemPop = false;
				}

				ImGui::PopID();
				++itr;
			}
		}

		if ( popID == 0 )
		{
			if ( ImGui::BeginPopupContextWindow() )
			{
				if ( m_bItemCut && !m_sFilepathToAction.empty() )
				{
					if ( ImGui::Selectable( "Paste" ) )
					{
						m_pFileDispatcher->EnqueueEvent( FileEvent{ .eAction = EFileAction::Paste } );
					}
				}
				else
				{
					ImGui::BeginDisabled();
					ImGui::Selectable( "Paste" );
					ImGui::EndDisabled();
				}
				ImGui::EndPopup();
			}
		}

		ImGui::EndTable();
	}

	OpenDeletePopup();

	ImGui::End();
}

void ContentDisplay::DrawToolbar()
{
	ImGui::Separator();

	if ( ImGui::Button( ICON_FA_FOLDER_PLUS ) )
	{
	}
	ImGui::ItemToolTip( "Create Folder" );
	ImGui::SameLine( 0.f, 16.f );

	const auto& savedPath = MAIN_REGISTRY().GetContext<std::shared_ptr<SaveProject>>()->sProjectPath;
	std::string pathStr{ m_CurrentDir.string() };
	std::string pathToSplit = pathStr.substr( pathStr.find( savedPath ) + savedPath.size() );
	auto dir = SplitStr( pathToSplit, PATH_SEPARATOR );
	for ( size_t i = 0; i < dir.size(); i++ )
	{
		if ( ImGui::Button( dir[ i ].c_str() ) )
		{
			std::string pathChange = pathStr.substr( 0, pathStr.find( dir[ i ] ) + dir[ i ].size() );
			m_CurrentDir = fs::path{ pathChange };
		}

		ImGui::SameLine();
		ImGui::PushStyleColor( ImGuiCol_Button, BLACK_TRANSPARENT );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, BLACK_TRANSPARENT );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, BLACK_TRANSPARENT );
		ImGui::Button( ICON_FA_ANGLE_RIGHT, { 16.f, 18.f } );
		ImGui::PopStyleColor( 3 );

		if ( i < dir.size() - 1 )
			ImGui::SameLine();
	}

	ImGui::Separator();
}

void ContentDisplay::CopyDroppedFile( const std::string& sFileToCopy, const std::filesystem::path& droppedPath )
{
	if ( droppedPath.empty() )
		return;

	std::error_code ec;
	if ( !fs::exists( droppedPath, ec ) )
	{
		SCION_ERROR( "Failed to copy dropped file: {} - {}", sFileToCopy, ec.message() );
		return;
	}

	fs::path source{ sFileToCopy };
	fs::path destination = fs::is_directory( droppedPath ) ? droppedPath : droppedPath.parent_path();
	destination /= source.filename();

	SCION_LOG( "Source: {}", sFileToCopy );
	SCION_LOG( "Destination {}", destination.string() );

	if ( fs::is_directory( source ) )
	{
		fs::copy( source, destination, fs::copy_options::recursive, ec );
		if ( ec )
		{
			SCION_ERROR( "Failed to copy the director [{}] -- {}", sFileToCopy, ec.message() );
		}
	}
	else if ( !fs::copy_file( source, destination, ec ) )
	{
		SCION_ERROR( "Failed to copy the file [{}] -- {}", sFileToCopy, ec.message() );
	}
}

void ContentDisplay::MoveFolderOrFile( const std::filesystem::path& movedPath, const std::filesystem::path& path )
{
	if ( !fs::is_directory( movedPath ) )
	{
		fs::rename( movedPath, path / movedPath.filename() );
	}
	else
	{
		auto foundFolder = fs::path{ GET_SUBSTRING( movedPath.string(), std::string{ PATH_SEPARATOR } ) };
		try
		{
			if ( !fs::is_empty( movedPath ) )
			{
				for ( const auto& entry : fs::recursive_directory_iterator( movedPath ) )
				{
					auto newPath = path / foundFolder;
					if ( !fs::exists( newPath ) )
						fs::create_directory( newPath );

					newPath /= entry.path().filename();
					fs::rename( entry.path(), newPath );
				}
			}
			else
			{
				fs::rename( movedPath, path / foundFolder );
			}
		}
		catch ( const fs::filesystem_error& error )
		{
			SCION_ERROR( "Failed to move folder and files - {}", error.what() );
		}
		// If the directory is empty, remove it
		if ( fs::exists( movedPath ) && fs::is_empty( movedPath ) )
			fs::remove( movedPath );
	}
}

void ContentDisplay::HandleFileEvent( const SCION_EDITOR::Events::FileEvent& fileEvent )
{
	if ( fileEvent.sFilepath.empty() || fileEvent.eAction == EFileAction::NoAction )
		return;

	switch ( fileEvent.eAction )
	{
	case EFileAction::Delete: {
		fs::path path{ fileEvent.sFilepath };
		if ( fs::is_directory( path ) )
			fs::remove_all( path );
		else if ( fs::is_regular_file( path ) )
			fs::remove( path );
		break;
	}
	case EFileAction::Paste: {
		if ( fs::is_directory( m_CurrentDir ) )
		{
			MoveFolderOrFile( m_sFilepathToAction, m_CurrentDir );
			m_bItemCut = false;
			m_sFilepathToAction.clear();
		}
		break;
	}
	case EFileAction::FileDropped: {
		if ( !m_bWindowHovered || fileEvent.sFilepath.empty() )
			break;

		CopyDroppedFile( fileEvent.sFilepath, m_CurrentDir );

		SCION_LOG( "Dropped file: {}", fileEvent.sFilepath );
		break;
	}
	}
}

void ContentDisplay::OpenDeletePopup()
{
	if ( m_eFileAction != Events::EFileAction::Delete )
		return;

	ImGui::OpenPopup( "Delete" );

	if ( ImGui::BeginPopupModal( "Delete" ) )
	{
		ImGui::Text( "This cannot be undone. Are you sure?" );
		if ( ImGui::Button( "Ok" ) )
		{
			m_pFileDispatcher->EnqueueEvent(
				FileEvent{ .eAction = EFileAction::Delete, .sFilepath = m_sFilepathToAction } );
			m_sFilepathToAction.clear();
			m_eFileAction = Events::EFileAction::NoAction;
		}
		ImGui::SameLine();
		if ( ImGui::Button( "Cancel" ) )
		{
			m_eFileAction = Events::EFileAction::NoAction;
		}
		ImGui::EndPopup();
	}
}

} // namespace SCION_EDITOR
