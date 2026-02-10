#include "editor/utilities/EditorState.h"
#include "ScionFilesystem/Serializers/JSONSerializer.h"
#include "Core/CoreUtilities/ProjectInfo.h"

#include "Logger/Logger.h"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

using namespace Scion::Filesystem;

namespace Scion::Editor
{

bool EditorState::Save( Scion::Core::ProjectInfo& projectInfo )
{
	const auto optEditorConfigFolder = projectInfo.TryGetFolderPath( Scion::Core::EProjectFolderType::EditorConfig );
	if ( !optEditorConfigFolder )
	{
		SCION_ERROR( "Failed to save editor state. Editor config path is invalid" );
		return false;
	}

	if ( !fs::exists( *optEditorConfigFolder ) )
	{
		SCION_ERROR( "Failed to save editor state at path [{}]", optEditorConfigFolder->string() );
		return false;
	}

	std::unique_ptr<JSONSerializer> pSerializer{ nullptr };
	fs::path editorStatePath = *optEditorConfigFolder / "editor_state.config";

	try
	{
		pSerializer = std::make_unique<JSONSerializer>( editorStatePath.string() );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to save editor state to file [{}] - [{}]", editorStatePath.string(), ex.what() );
		return false;
	}

	pSerializer->StartDocument();
	pSerializer->StartNewObject( "warnings" );
	pSerializer->AddKeyValuePair( "warning", std::string{ "THIS FILE IS ENGINE GENERATED." } )
		.AddKeyValuePair( "warning", std::string{ "DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING." } )
		.EndObject(); // Warnings

	pSerializer->StartNewObject( "editor_state" )
		.AddKeyValuePair( "active_displays", activeDisplays )
		.AddKeyValuePair( "gridSnap", bEnableGridsnap )
		.AddKeyValuePair( "showAnimations", bShowAnimations )
		.AddKeyValuePair( "showCollisions", bShowCollisions )
		.EndObject(); // EditorState

	return pSerializer->EndDocument();
}

bool EditorState::Load( Scion::Core::ProjectInfo& projectInfo )
{
	const auto optEditorConfigFolder = projectInfo.TryGetFolderPath( Scion::Core::EProjectFolderType::EditorConfig );
	if ( !optEditorConfigFolder )
	{
		SCION_ERROR( "Failed to load editor state. Editor config path is invalid" );
		return false;
	}

	fs::path editorStatePath = *optEditorConfigFolder / "editor_state.config";
	if ( !fs::exists( editorStatePath ) )
	{
		SCION_ERROR( "Failed to load editor state at path [{}]", editorStatePath.string() );
		return false;
	}

	std::ifstream editorStateFile;
	editorStateFile.open( editorStatePath.string() );

	if ( !editorStateFile.is_open() )
	{
		SCION_ERROR( "Failed to open project file [{}]", editorStatePath.string() );
		return false;
	}

	std::stringstream ss;
	ss << editorStateFile.rdbuf();
	std::string contents = ss.str();
	rapidjson::StringStream jsonStr{ contents.c_str() };

	rapidjson::Document doc;
	doc.ParseStream( jsonStr );

	if ( doc.HasParseError() || !doc.IsObject() )
	{
		SCION_ERROR( "Failed to load editor state: File: [{}] is not valid JSON. - {} - {}",
					 editorStatePath.string(),
					 rapidjson::GetParseError_En( doc.GetParseError() ),
					 doc.GetErrorOffset() );

		return false;
	}

	// Get the project data
	if ( !doc.HasMember( "editor_state" ) )
	{
		SCION_ERROR( "Failed to load project: File [{}] - Expecting \"editor_state\" member in project file.",
					 editorStatePath.string() );
		return false;
	}

	const rapidjson::Value& editorState = doc[ "editor_state" ];

	activeDisplays = editorState[ "active_displays" ].GetUint64();
	bShowAnimations = editorState[ "showAnimations" ].GetBool();
	bShowCollisions = editorState[ "showCollisions" ].GetBool();
	bEnableGridsnap = editorState[ "gridSnap" ].GetBool();

	return true;
}

bool EditorState::ImportState( const std::string& sImportedStateFile, Scion::Core::ProjectInfo& projectInfo )
{
	return false;
}

bool EditorState::CreateEditorStateFile( Scion::Core::ProjectInfo& projectInfo )
{
	const auto optEditorConfigFolder = projectInfo.TryGetFolderPath( Scion::Core::EProjectFolderType::EditorConfig );
	if ( !optEditorConfigFolder )
	{
		SCION_ERROR( "Failed to save editor state. Editor config path is invalid" );
		return false;
	}

	if ( !fs::exists( *optEditorConfigFolder ) )
	{
		SCION_ERROR( "Failed to save editor state at path [{}]", optEditorConfigFolder->string() );
		return false;
	}

	std::unique_ptr<JSONSerializer> pSerializer{ nullptr };
	fs::path editorStatePath = *optEditorConfigFolder / "editor_state.config";

	try
	{
		pSerializer = std::make_unique<JSONSerializer>( editorStatePath.string() );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to save editor state to file [{}] - [{}]", editorStatePath.string(), ex.what() );
		return false;
	}

	pSerializer->StartDocument();
	pSerializer->StartNewObject( "warnings" );
	pSerializer->AddKeyValuePair( "warning", std::string{ "THIS FILE IS ENGINE GENERATED." } )
		.AddKeyValuePair( "warning", std::string{ "DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING." } )
		.EndObject(); // Warnings

	pSerializer->StartNewObject( "editor_state" )
		.AddKeyValuePair(
			"active_displays",
			static_cast<uint64_t>( EDisplay::Hierarchy ) | static_cast<uint64_t>( EDisplay::AssetBrowser ) |
				static_cast<uint64_t>( EDisplay::Console ) | static_cast<uint64_t>( EDisplay::TilemapView ) )
		.AddKeyValuePair( "gridSnap", true )
		.AddKeyValuePair( "showAnimations", false )
		.AddKeyValuePair( "showCollisions", false )
		.EndObject(); // EditorState

	return pSerializer->EndDocument();
}

} // namespace Scion::Editor
