#include "ContentDisplay.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "ScionUtilities/ScionUtilities.h"
#include "ScionUtilities/HelperUtilities.h"
#include "ScionFilesystem/Dialogs/FileDialog.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "Logger/Logger.h"

#include "editor/events/EditorEventTypes.h"
#include "Core/CoreUtilities/SaveProject.h"
#include "Core/Events/EventDispatcher.h"

#include "ScionFilesystem/Process/FileProcessor.h"
#include "ScionFilesystem/Serializers/LuaSerializer.h"
#include <imgui.h>

using namespace SCION_EDITOR::Events;
using namespace SCION_FILESYSTEM;

namespace fs = std::filesystem;

namespace SCION_EDITOR
{
ContentDisplay::ContentDisplay()
	: m_pFileDispatcher{ std::make_unique<SCION_CORE::Events::EventDispatcher>() }
	, m_CurrentDir{ MAIN_REGISTRY().GetContext<std::shared_ptr<SCION_CORE::SaveProject>>()->sProjectPath + "content" }
	, m_sFilepathToAction{}
	, m_Selected{ -1 }
	, m_eFileAction{ Events::EFileAction::NoAction }
	, m_eCreateAction{ Events::EContentCreateAction::NoAction }
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
		// SCION_ERROR( "NumCols is zero!" );
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
					ImGui::ImageButton(
						contentBtn.c_str(), (ImTextureID)(intptr_t)icon->GetID(), ImVec2{ 80.f, 80.f } );
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
					ImGui::ImageButton(
						contentBtn.c_str(), (ImTextureID)(intptr_t)icon->GetID(), ImVec2{ 80.f, 80.f } );
					ImGui::PopStyleVar( 1 );

					if ( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( 0 ) )
					{
						SCION_FILESYSTEM::FileProcessor fp{};
						if ( !fp.OpenApplicationFromFile( path.string(), {} ) )
						{
							SCION_ERROR( "Failed to open file {}", path.string() );
						}
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
						ImGui::Selectable( ICON_FA_CUT " Cut" );
						ImGui::EndDisabled();
					}
					else
					{
						if ( ImGui::Selectable( ICON_FA_CUT " Cut" ) )
						{
							m_sFilepathToAction = path.string();
							m_bItemCut = true;
						}

						if ( ImGui::Selectable( ICON_FA_TRASH " Delete" ) )
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
					if ( ImGui::Selectable( ICON_FA_PASTE " Paste" ) )
					{
						m_pFileDispatcher->EnqueueEvent( FileEvent{ .eAction = EFileAction::Paste } );
					}
				}
				else
				{
					ImGui::BeginDisabled();
					ImGui::Selectable( ICON_FA_PASTE " Paste" );
					ImGui::EndDisabled();
				}

				if ( ImGui::Selectable( ICON_FA_FOLDER " New Folder" ) )
				{
					m_sFilepathToAction = m_CurrentDir.string();
					m_eCreateAction = Events::EContentCreateAction::Folder;
				}

				if ( ImGui::TreeNode( "Lua Objects" ) )
				{
					if ( ImGui::Selectable( ICON_FA_FILE " Create Lua Class" ) )
					{
						m_sFilepathToAction = m_CurrentDir.string();
						m_eCreateAction = Events::EContentCreateAction::LuaClass;
					}
					ImGui::ItemToolTip( "Generates an empty lua class." );

					if ( ImGui::Selectable( ICON_FA_FILE " Create Lua Table" ) )
					{
						m_sFilepathToAction = m_CurrentDir.string();
						m_eCreateAction = Events::EContentCreateAction::LuaTable;
					}
					ImGui::ItemToolTip( "Generates an empty lua table." );

					if ( ImGui::Selectable( ICON_FA_FILE " Create Lua File" ) )
					{
						m_sFilepathToAction = m_CurrentDir.string();
						m_eCreateAction = Events::EContentCreateAction::EmptyLuaFile;
					}
					ImGui::ItemToolTip( "Generates an empty lua File." );

					ImGui::TreePop();
				}

				ImGui::EndPopup();
			}
		}

		ImGui::EndTable();
	}

	HandlePopups();

	ImGui::End();
}

void ContentDisplay::DrawToolbar()
{
	ImGui::Separator();

	if ( ImGui::Button( ICON_FA_FOLDER_PLUS ) )
	{
		m_sFilepathToAction = m_CurrentDir.string();
		m_eCreateAction = Events::EContentCreateAction::Folder;
	}
	ImGui::ItemToolTip( "Create Folder" );
	ImGui::SameLine( 0.f, 16.f );

	const auto& savedPath = MAIN_REGISTRY().GetContext<std::shared_ptr<SCION_CORE::SaveProject>>()->sProjectPath;
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
		ImGui::Button( fmt::format( "{}##{}", ICON_FA_ANGLE_RIGHT, i ).c_str(), { 16.f, 18.f } );
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
		{
			std::unordered_set<std::string> setFilesToCheck;
			// Collect all regular file paths inside the directory before deleting it
			for ( const auto& entry : fs::recursive_directory_iterator( path ) )
			{
				if ( entry.is_regular_file() )
				{
					setFilesToCheck.insert( entry.path().string() );
				}
			}

			// Recursively delete the directory and its contents
			fs::remove_all( path );

			// Remove each file from the asset manager
			for ( const auto& sFilepath : setFilesToCheck )
			{
				if ( !ASSET_MANAGER().DeleteAssetFromPath( sFilepath ) )
				{
					SCION_ERROR( "Failed to remove asset from asset manager. [{}]", sFilepath );
				}
			}
		}
		else if ( fs::is_regular_file( path ) )
		{
			// Remove the file from disk
			if ( fs::remove( path ) )
			{
				// Remove the file from the asset manager
				if ( !ASSET_MANAGER().DeleteAssetFromPath( path.string() ) )
				{
					SCION_ERROR( "Failed to remove asset from asset manager. [{}]", path.string() );
				}
			}
		}
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

void ContentDisplay::HandleCreateEvent( const SCION_EDITOR::Events::ContentCreateEvent& createEvent )
{
	if ( createEvent.eAction == EContentCreateAction::NoAction )
		return;

	switch ( createEvent.eAction )
	{
	case EContentCreateAction::Folder: OpenCreateFolderPopup(); break;
	}
}

void ContentDisplay::HandlePopups()
{
	if ( m_eFileAction != EFileAction::NoAction )
	{
		switch ( m_eFileAction )
		{
		case EFileAction::Delete: OpenDeletePopup(); break;
		}
	}

	if ( m_eCreateAction != EContentCreateAction::NoAction )
	{
		switch ( m_eCreateAction )
		{
		case EContentCreateAction::Folder: OpenCreateFolderPopup(); break;
		case EContentCreateAction::LuaClass: OpenCreateLuaClassPopup(); break;
		case EContentCreateAction::LuaTable: OpenCreateLuaTablePopup(); break;
		case EContentCreateAction::EmptyLuaFile: OpenCreateEmptyLuaFilePopup(); break;
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

void ContentDisplay::OpenCreateFolderPopup()
{
	if ( m_eCreateAction != EContentCreateAction::Folder )
		return;

	ImGui::OpenPopup( "Create Folder" );

	if ( ImGui::BeginPopupModal( "Create Folder" ) )
	{
		static std::string newFolderStr{};
		char temp[ 256 ];
		memset( temp, 0, sizeof( temp ) );
#ifdef _WIN32
		strcpy_s( temp, newFolderStr.c_str() );
#else
		strcpy( temp, newFolderStr.c_str() );
#endif
		bool bNameEntered{ false }, bExit{ false };

		ImGui::Text( "folder name" );
		ImGui::SameLine();

		if ( !ImGui::IsAnyItemActive() )
			ImGui::SetKeyboardFocusHere();

		if ( ImGui::InputText( "##folder name", temp, sizeof( temp ), ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			newFolderStr = std::string{ temp };
			bNameEntered = true;
		}
		else if ( ImGui::IsKeyPressed( ImGuiKey_Escape ) )
		{
			bExit = true;
		}

		static std::string errorText{};
		if ( bNameEntered && !newFolderStr.empty() )
		{
			std::string folderPathStr = m_CurrentDir.string() + PATH_SEPARATOR + newFolderStr;
			std::error_code error{};
			if ( !fs::create_directory( fs::path{ folderPathStr }, error ) )
			{
				SCION_ERROR( "Failed to create new folder - {}", error.message() );
				errorText = "Failed to create new folder - " + error.message();
			}
			else
			{
				m_eCreateAction = EContentCreateAction::NoAction;
				m_sFilepathToAction.clear();
				newFolderStr.clear();
				errorText.clear();
				ImGui::CloseCurrentPopup();
			}
		}

		if ( ImGui::Button( "Cancel" ) || bExit )
		{
			m_eCreateAction = EContentCreateAction::NoAction;
			m_sFilepathToAction.clear();
			errorText.clear();
			newFolderStr.clear();
			ImGui::CloseCurrentPopup();
		}

		if ( !errorText.empty() )
		{
			ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f }, errorText.c_str() );
		}

		ImGui::EndPopup();
	}
}

void ContentDisplay::OpenCreateLuaClassPopup()
{
	if ( m_eCreateAction != EContentCreateAction::LuaClass )
		return;

	ImGui::OpenPopup( "Create Lua Class" );

	if ( ImGui::BeginPopupModal( "Create Lua Class" ) )
	{
		char buffer[ 256 ];
		static std::string className{};
		memset( buffer, 0, sizeof( buffer ) );
#ifdef _WIN32
		strcpy_s( buffer, className.c_str() );
#else
		strcpy( buffer, className.c_str() );
#endif
		bool bNameEntered{ false }, bExit{ false };
		ImGui::Text( "Class Name" );
		ImGui::SameLine();

		if ( !ImGui::IsAnyItemActive() )
			ImGui::SetKeyboardFocusHere();

		if ( ImGui::InputText( "##ClassName", buffer, sizeof( buffer ), ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			className = std::string{ buffer };
			bNameEntered = true;
		}
		else if ( ImGui::IsKeyPressed( ImGuiKey_Escape ) )
		{
			bExit = true;
		}

		static std::string errorText{};

		if ( bNameEntered && !className.empty() )
		{
			std::string filename = m_sFilepathToAction + PATH_SEPARATOR + className + ".lua";

			if ( fs::exists( fs::path{ filename } ) )
			{
				SCION_ERROR( "Class file: [{}] already exists at [{}]", className, filename );
				errorText = "Class file: [" + className + "] already exists at [" + filename + "]";
			}
			else
			{
				LuaSerializer lw{ filename };

				lw.AddWords( className + " = {}" )
					.AddWords( className + ".__index = " + className, true )
					.AddWords( "function " + className + ":Create(params)", true )
					.AddWords( "local this = {}", true, true )
					.AddWords( "setmetatable(this, self)", true, true )
					.AddWords( "return this", true, true )
					.AddWords( "end", true );

				errorText.clear();
				className.clear();
				m_eCreateAction = EContentCreateAction::NoAction;
				m_sFilepathToAction.clear();
				ImGui::CloseCurrentPopup();
			}
		}

		if ( ImGui::Button( "Cancel" ) || bExit )
		{
			ImGui::CloseCurrentPopup();
			m_eCreateAction = EContentCreateAction::NoAction;
			className.clear();
			errorText.clear();
			m_sFilepathToAction.clear();
		}

		if ( !errorText.empty() )
		{
			ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f }, errorText.c_str() );
		}

		ImGui::EndPopup();
	}
}

void ContentDisplay::OpenCreateLuaTablePopup()
{
	if ( m_eCreateAction != EContentCreateAction::LuaTable )
		return;

	ImGui::OpenPopup( "Create Lua Table" );

	if ( ImGui::BeginPopupModal( "Create Lua Table" ) )
	{
		char buffer[ 256 ];
		static std::string tableName{};
		memset( buffer, 0, sizeof( buffer ) );
#ifdef _WIN32
		strcpy_s( buffer, tableName.c_str() );
#else
		strcpy( buffer, tableName.c_str() );
#endif
		bool bNameEntered{ false }, bExit{ false };
		ImGui::Text( "Table Name" );
		ImGui::SameLine();

		if ( !ImGui::IsAnyItemActive() )
			ImGui::SetKeyboardFocusHere();

		if ( ImGui::InputText( "##TableName", buffer, sizeof( buffer ), ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			tableName = std::string{ buffer };
			bNameEntered = true;
		}
		else if ( ImGui::IsKeyPressed( ImGuiKey_Escape ) )
		{
			bExit = true;
		}

		static std::string errorText{};

		if ( bNameEntered && !tableName.empty() )
		{
			std::string filename = m_sFilepathToAction + PATH_SEPARATOR + tableName + ".lua";

			if ( fs::exists( fs::path{ filename } ) )
			{
				SCION_ERROR( "Table file: [{}] already exists at [{}]", tableName, filename );
				errorText = "Table file: [" + tableName + "] already exists at [" + filename + "]";
			}
			else
			{
				LuaSerializer lw{ filename };

				lw.StartNewTable( tableName ).EndTable().FinishStream();

				errorText.clear();
				tableName.clear();
				m_eCreateAction = EContentCreateAction::NoAction;
				m_sFilepathToAction.clear();
				ImGui::CloseCurrentPopup();
			}
		}

		if ( ImGui::Button( "Cancel" ) || bExit )
		{
			ImGui::CloseCurrentPopup();
			m_eCreateAction = EContentCreateAction::NoAction;
			tableName.clear();
			errorText.clear();
			m_sFilepathToAction.clear();
		}

		if ( !errorText.empty() )
		{
			ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f }, errorText.c_str() );
		}

		ImGui::EndPopup();
	}
}

void ContentDisplay::OpenCreateEmptyLuaFilePopup()
{
	if ( m_eCreateAction != EContentCreateAction::EmptyLuaFile )
		return;

	ImGui::OpenPopup( "Create Lua File" );

	if ( ImGui::BeginPopupModal( "Create Lua File" ) )
	{
		char buffer[ 256 ];
		static std::string tableName{};
		memset( buffer, 0, sizeof( buffer ) );
#ifdef _WIN32
		strcpy_s( buffer, tableName.c_str() );
#else
		strcpy( buffer, tableName.c_str() );
#endif
		bool bNameEntered{ false }, bExit{ false };
		ImGui::Text( "FileName" );
		ImGui::SameLine();

		if ( !ImGui::IsAnyItemActive() )
			ImGui::SetKeyboardFocusHere();

		if ( ImGui::InputText( "##FileName", buffer, sizeof( buffer ), ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			tableName = std::string{ buffer };
			bNameEntered = true;
		}
		else if ( ImGui::IsKeyPressed( ImGuiKey_Escape ) )
		{
			bExit = true;
		}

		static std::string errorText{};

		if ( bNameEntered && !tableName.empty() )
		{
			std::string filename = m_sFilepathToAction + PATH_SEPARATOR + tableName + ".lua";

			if ( fs::exists( fs::path{ filename } ) )
			{
				SCION_ERROR( "File: [{}] already exists at [{}]", tableName, filename );
				errorText = "File: [" + tableName + "] already exists at [" + filename + "]";
			}
			else
			{
				LuaSerializer lw{ filename };
				lw.FinishStream();

				errorText.clear();
				tableName.clear();
				m_eCreateAction = EContentCreateAction::NoAction;
				m_sFilepathToAction.clear();
				ImGui::CloseCurrentPopup();
			}
		}

		if ( ImGui::Button( "Cancel" ) || bExit )
		{
			ImGui::CloseCurrentPopup();
			m_eCreateAction = EContentCreateAction::NoAction;
			tableName.clear();
			errorText.clear();
			m_sFilepathToAction.clear();
		}

		if ( !errorText.empty() )
		{
			ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f }, errorText.c_str() );
		}

		ImGui::EndPopup();
	}
}

} // namespace SCION_EDITOR
