#include "ProjectLoader.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "editor/utilities/SaveProject.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "Logger/Logger.h"

#include "ScionFilesystem/Serializers/JSONSerializer.h"
#include "ScionFilesystem/Serializers/LuaSerializer.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace SCION_FILESYSTEM;

namespace SCION_EDITOR
{

bool ProjectLoader::CreateNewProject( const std::string& sProjectName, const std::string& sFilepath )
{
	auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<SaveProject>>();
	SCION_ASSERT( pSaveProject && "Save project must exist." );

	// Create the game filepath
	std::string sGameFilepath =
		fmt::format( "{}{}{}{}{}", sFilepath, PATH_SEPARATOR, sProjectName, PATH_SEPARATOR, "SCION_2D" );

	if ( fs::is_directory( sGameFilepath ) )
	{
		SCION_ERROR( "Project [{}] at [{}] already exists.", sProjectName, sFilepath );
		return false;
	}

	char sep{ PATH_SEPARATOR };
	sGameFilepath += sep;
	std::error_code ec;
	if ( !fs::create_directories( sGameFilepath + "content", ec ) ||
		 !fs::create_directories( sGameFilepath + "content" + sep + "scripts", ec ) ||
		 !fs::create_directories( sGameFilepath + "content" + sep + "assets", ec ) ||
		 !fs::create_directories( sGameFilepath + "content" + sep + "assets" + sep + "soundfx", ec ) ||
		 !fs::create_directories( sGameFilepath + "content" + sep + "assets" + sep + "music", ec ) ||
		 !fs::create_directories( sGameFilepath + "content" + sep + "assets" + sep + "textures", ec ) ||
		 !fs::create_directories( sGameFilepath + "content" + sep + "assets" + sep + "shaders", ec ) ||
		 !fs::create_directories( sGameFilepath + "content" + sep + "assets" + sep + "fonts", ec ) ||
		 !fs::create_directories( sGameFilepath + "content" + sep + "assets" + sep + "scenes", ec ) )
	{
		SCION_ERROR( "Failed to create directories - {}", ec.message() );
		// TODO: Delete any created directories??
		return false;
	}

	pSaveProject->sProjectName = sProjectName;
	pSaveProject->sProjectPath = sGameFilepath;

	return CreateProjectFile( pSaveProject->sProjectName, pSaveProject->sProjectPath );
}

bool ProjectLoader::LoadProject( const std::string& sFilepath )
{
	std::ifstream procFile;
	procFile.open( sFilepath );

	if ( !procFile.is_open() )
	{
		SCION_ERROR( "Failed to open project file [{}]", sFilepath );
		return false;
	}

	std::stringstream ss;
	ss << procFile.rdbuf();
	std::string contents = ss.str();
	rapidjson::StringStream jsonStr{ contents.c_str() };

	rapidjson::Document doc;
	doc.ParseStream( jsonStr );

	if ( doc.HasParseError() || !doc.IsObject() )
	{
		SCION_ERROR( "Failed to load Project: File: [{}] is not valid JSON. - {} - {}",
					 sFilepath,
					 rapidjson::GetParseError_En( doc.GetParseError() ),
					 doc.GetErrorOffset() );
		return false;
	}

	// Get the project data
	if ( !doc.HasMember( "project_data" ) )
	{
		SCION_ERROR( "Failed to load project: File [{}] - Expecting \"project_data\" member in project file.",
					 sFilepath );
		return false;
	}

	auto& mainRegistry = MAIN_REGISTRY();
	auto& pSaveProject = mainRegistry.GetContext<std::shared_ptr<SaveProject>>();

	SCION_ASSERT( pSaveProject && "Save Project must be valid!" );

	// We need the project filepath saved!
	pSaveProject->sProjectFilePath = sFilepath;

	const rapidjson::Value& projectData = doc[ "project_data" ];

	// Set the project name. The actual project name might be different to the project files name.
	pSaveProject->sProjectName = projectData[ "project_name" ].GetString();

	// We need to load all the assets
	if ( !projectData.HasMember( "assets" ) )
	{
		SCION_ERROR( "Failed to load project: File [{}] - Expecting \"assets\" member in project file.", sFilepath );
		return false;
	}

	// Get Content Path
	fs::path filePath{ sFilepath };
	std::string sContentPath = filePath.parent_path().string();

	// Get the project path before we adjust it to the content path
	pSaveProject->sProjectPath = sContentPath + PATH_SEPARATOR;
	CORE_GLOBALS().SetProjectPath( pSaveProject->sProjectPath );

	sContentPath += PATH_SEPARATOR;
	sContentPath += "content";
	sContentPath += PATH_SEPARATOR;

	// Check to see if there is a main lua path
	if ( projectData.HasMember( "main_lua_script" ) )
	{
		pSaveProject->sMainLuaScript = sContentPath + projectData[ "main_lua_script" ].GetString();
	}

	const rapidjson::Value& assets = projectData[ "assets" ];
	auto& assetManager = ASSET_MANAGER();

	// Load all textures into the asset manager
	if ( assets.HasMember( "textures" ) )
	{
		const rapidjson::Value& textures = assets[ "textures" ];

		if ( !textures.IsArray() )
		{
			SCION_ERROR( "Failed to load project: File [{}] - Expecting \"textures \" must be an array.", sFilepath );
			return false;
		}

		for ( const auto& jsonTexture : textures.GetArray() )
		{
			// Assets path's should be saved as follows "assets/[asset_type]/[extra_folders opt]/file"

			std::string sTextureName{ jsonTexture[ "name" ].GetString() };
			std::string sTexturePath{ sContentPath + jsonTexture[ "path" ].GetString() };

			if ( !assetManager.AddTexture( sTextureName,
										   sTexturePath,
										   jsonTexture[ "bPixelArt" ].GetBool(),
										   jsonTexture[ "bTilemap" ].GetBool() ) )
			{
				SCION_ERROR( "Failed to load texture [{}] at path [{}]", sTextureName, sTexturePath );
				// Should we stop loading or finish??
			}
		}
	}

	// Load all soundfx to the asset manager
	if ( assets.HasMember( "soundfx" ) )
	{
		const rapidjson::Value& soundfx = assets[ "soundfx" ];

		if ( !soundfx.IsArray() )
		{
			SCION_ERROR( "Failed to load project: File [{}] - Expecting \"soundfx\" must be an array.", sFilepath );
			return false;
		}

		for ( const auto& jsonSoundFx : soundfx.GetArray() )
		{
			std::string sSoundFxName{ jsonSoundFx[ "name" ].GetString() };
			std::string sSoundFxPath{ sContentPath + jsonSoundFx[ "path" ].GetString() };

			if ( !assetManager.AddSoundFx( sSoundFxName, sSoundFxPath ) )
			{
				SCION_ERROR( "Failed to load soundfx [{}] at path [{}]", sSoundFxName, sSoundFxPath );
				// Should we stop loading or finish??
			}
		}
	}

	// Load all music to the asset manager
	if ( assets.HasMember( "music" ) )
	{
		const rapidjson::Value& music = assets[ "music" ];

		if ( !music.IsArray() )
		{
			SCION_ERROR( "Failed to load project: File [{}] - Expecting \"music\" must be an array.", sFilepath );
			return false;
		}

		for ( const auto& jsonMusic : music.GetArray() )
		{
			std::string sMusicName{ jsonMusic[ "name" ].GetString() };
			std::string sMusicPath{ sContentPath + jsonMusic[ "path" ].GetString() };

			if ( !assetManager.AddMusic( sMusicName, sMusicPath ) )
			{
				SCION_ERROR( "Failed to load music [{}] at path [{}]", sMusicName, sMusicPath );
				// Should we stop loading or finish??
			}
		}
	}

	// Load all fonts to the asset manager
	if ( assets.HasMember( "fonts" ) )
	{
		const rapidjson::Value& fonts = assets[ "fonts" ];

		if ( !fonts.IsArray() )
		{
			SCION_ERROR( "Failed to load project: File [{}] - Expecting \"fonts\" must be an array.", sFilepath );
			return false;
		}

		for ( const auto& jsonFonts : fonts.GetArray() )
		{
			std::string sFontName{ jsonFonts[ "name" ].GetString() };
			std::string sFontPath{ sContentPath + jsonFonts[ "path" ].GetString() };

			if ( !assetManager.AddFont( sFontName, sFontPath, jsonFonts[ "fontSize" ].GetFloat() ) )
			{
				SCION_ERROR( "Failed to load fonts [{}] at path [{}]", sFontName, sFontPath );
				// Should we stop loading or finish??
			}
		}
	}

	// Load all scenes to the scene manager
	if ( assets.HasMember( "scenes" ) )
	{
		const rapidjson::Value& scenes = assets[ "scenes" ];

		if ( !scenes.IsArray() )
		{
			SCION_ERROR( "Failed to load project: File [{}] - Expecting \"scenes\" must be an array.", sFilepath );
			return false;
		}

		auto& sceneManager = SCENE_MANAGER();

		for ( const auto& jsonScenes : scenes.GetArray() )
		{
			std::string sSceneName{ jsonScenes[ "name" ].GetString() };
			std::string sSceneDataPath{ sContentPath + jsonScenes[ "sceneData" ].GetString() };

			if ( !sceneManager.AddScene( sSceneName, sSceneDataPath ) )
			{
				SCION_ERROR( "Failed to load scene: {}", sSceneName );
			}
		}
	}

	return true;
}

bool ProjectLoader::SaveLoadedProject( SaveProject& save )
{
	if ( !fs::exists( save.sProjectFilePath ) )
	{
		SCION_ERROR( "Failed to save project file for [{}] at path [{}]", save.sProjectName, save.sProjectFilePath );
		return false;
	}

	std::unique_ptr<JSONSerializer> pSerializer{ nullptr };

	try
	{
		pSerializer = std::make_unique<JSONSerializer>( save.sProjectFilePath );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to save tilemap [{}] - [{}]", save.sProjectFilePath, ex.what() );
		return false;
	}

	auto& assetManager = ASSET_MANAGER();
	auto& sceneMananger = SCENE_MANAGER();

	if ( !sceneMananger.SaveAllScenes() )
	{
		SCION_ERROR( "Failed to save all scenes." );
	}

	pSerializer->StartDocument();
	pSerializer->StartNewObject( "warnings" );
	pSerializer->AddKeyValuePair( "warning", std::string{ "THIS FILE IS ENGINE GENERATED." } )
		.AddKeyValuePair( "warning", std::string{ "DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING." } );
	pSerializer->EndObject(); // Warnings

	pSerializer->StartNewObject( "project_data" )
		.AddKeyValuePair( "project_name", save.sProjectName )
		.AddKeyValuePair( "main_lua_script", save.sMainLuaScript.substr( save.sMainLuaScript.find( SCRIPTS ) ) )
		.StartNewObject( "assets" );

	pSerializer->StartNewArray( "textures" );
	for ( const auto& [ sName, pTexture ] : assetManager.GetAllTextures() )
	{
		if ( !pTexture || pTexture->IsEditorTexture() )
			continue;

		// Get Relative to assets path
		std::string sTexturePath = pTexture->GetPath().substr( pTexture->GetPath().find( ASSETS ) );
		SCION_ASSERT( !sTexturePath.empty() );

		pSerializer->StartNewObject()
			.AddKeyValuePair( "name", sName )
			.AddKeyValuePair( "path", sTexturePath )
			.AddKeyValuePair( "bPixelArt", pTexture->GetType() == SCION_RENDERING::Texture::TextureType::PIXEL )
			.AddKeyValuePair( "bTilemap", pTexture->IsTileset() )
			.EndObject();
	}
	pSerializer->EndArray(); // Textures

	pSerializer->StartNewArray( "soundfx" );
	for ( const auto& [ sName, pSound ] : assetManager.GetAllSoundFx() )
	{
		std::string sSoundFxPath = pSound->GetFilename().substr( pSound->GetFilename().find( ASSETS ) );
		pSerializer->StartNewObject()
			.AddKeyValuePair( "name", sName )
			.AddKeyValuePair( "path", sSoundFxPath )
			.EndObject();
	}
	pSerializer->EndArray(); // SoundFx

	pSerializer->StartNewArray( "music" );
	for ( const auto& [ sName, pMusic ] : assetManager.GetAllMusic() )
	{
		std::string sMusicPath = pMusic->GetFilename().substr( pMusic->GetFilename().find( ASSETS ) );
		pSerializer->StartNewObject()
			.AddKeyValuePair( "name", sName )
			.AddKeyValuePair( "path", sMusicPath )
			.EndObject();
	}
	pSerializer->EndArray(); // Music

	pSerializer->StartNewArray( "scenes" );

	for ( const auto& [ sName, pScene ] : sceneMananger.GetAllScenes() )
	{
		std::string sScenePath = pScene->GetSceneDataPath().substr( pScene->GetSceneDataPath().find( ASSETS ) );
		pSerializer->StartNewObject()
			.AddKeyValuePair( "name", sName )
			.AddKeyValuePair( "sceneData", sScenePath )
			.EndObject();
	}
	pSerializer->EndArray();  // Scenes
	pSerializer->EndObject(); // Assets
	pSerializer->EndObject(); // Project Data

	return pSerializer->EndDocument();
}

bool ProjectLoader::CreateProjectFile( const std::string& sProjectName, const std::string& sFilepath )
{
	if ( !fs::is_directory( sFilepath ) )
	{
		SCION_ERROR( "Failed to create project file for [{}] at path [{}]", sProjectName, sFilepath );
		return false;
	}

	if ( !CreateMainLuaScript( sProjectName, sFilepath ) )
	{
		SCION_ERROR( "Failed to create main lua script" );
		return false;
	}

	std::string sProjectFile{ sFilepath + sProjectName + S2D_PRJ_FILE_EXT };

	std::unique_ptr<JSONSerializer> pSerializer{ nullptr };

	try
	{
		pSerializer = std::make_unique<JSONSerializer>( sProjectFile );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to save tilemap [{}] - [{}]", sProjectFile, ex.what() );
		return false;
	}

	// We want to grab the project file path
	auto& pSaveFile = MAIN_REGISTRY().GetContext<std::shared_ptr<SaveProject>>();
	pSaveFile->sProjectFilePath = sProjectFile;

	pSerializer->StartDocument();
	pSerializer->StartNewObject( "warnings" );
	pSerializer->AddKeyValuePair( "warning", std::string{ "THIS FILE IS ENGINE GENERATED." } )
		.AddKeyValuePair( "warning", std::string{ "DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING." } );
	pSerializer->EndObject(); // Warnings

	pSerializer->StartNewObject( "project_data" )
		.AddKeyValuePair( "project_name", sProjectName )
		.AddKeyValuePair( "main_lua_file",
						  pSaveFile->sMainLuaScript.substr( pSaveFile->sMainLuaScript.find( SCRIPTS ) ) )
		.StartNewObject( "assets" )
		.StartNewArray( "textures" )
		.EndArray() // Textures
		.StartNewArray( "soundfx" )
		.EndArray() // SoundFx
		.StartNewArray( "music" )
		.EndArray() // Music
		.StartNewArray( "scenes" )
		.EndArray()			  // Scenes
		.EndObject();		  // Assets
	pSerializer->EndObject(); // Project Data

	return pSerializer->EndDocument();
}

bool ProjectLoader::CreateMainLuaScript( const std::string& sProjectName, const std::string& sFilepath )
{
	std::string sMainLuaFile =
		fmt::format( "{}{}{}{}{}main.lua", sFilepath, "content", PATH_SEPARATOR, "scripts", PATH_SEPARATOR );

	auto pLuaSerializer = std::make_unique<LuaSerializer>( sMainLuaFile );
	SCION_ASSERT( pLuaSerializer );

	// Save the main lua file path
	MAIN_REGISTRY().GetContext<std::shared_ptr<SaveProject>>()->sMainLuaScript = sMainLuaFile;

	pLuaSerializer->AddBlockComment( "\tMain Lua script. This is needed to run all scripts in the editor"
									 "\n\tGENERATED BY THE ENGINE ON PROJECT CREATION. DON'T CHANGE UNLESS "
									 "\n\tYOU KNOW WHAT YOU ARE DOING!" );

	pLuaSerializer->AddComment( "The engine looks for these two functions." )
		.AddComment( "Please add your code inside of the update and render functions as needed." );

	pLuaSerializer->StartNewTable( "main" )
		.StartNewTable( "1", true, true )
		.AddKeyValuePair( "update", "function() end", true, true )
		.EndTable()
		.StartNewTable( "2", true, true )
		.AddKeyValuePair( "render", "function() end", true, true )
		.EndTable()
		.EndTable();

	return pLuaSerializer->FinishStream();
}

} // namespace SCION_EDITOR
