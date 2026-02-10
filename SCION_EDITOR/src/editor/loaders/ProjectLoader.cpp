#include "editor/loaders/ProjectLoader.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/ProjectInfo.h"
#include "Core/CoreUtilities/Prefab.h"

#include "editor/utilities/EditorUtilities.h"
#include "editor/utilities/EditorState.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "Logger/Logger.h"

#include "ScionFilesystem/Serializers/JSONSerializer.h"
#include "ScionFilesystem/Serializers/LuaSerializer.h"

#include <Rendering/Essentials/Shader.h>
#include <Rendering/Essentials/Texture.h>
#include <Rendering/Essentials/Font.h>
#include <Sounds/Essentials/Music.h>
#include <Sounds/Essentials/SoundFX.h>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

using namespace Scion::Filesystem;

// clang-format off
static const std::map<Scion::Core::EProjectFolderType, std::string> mapProjectDirs = {
		{ Scion::Core::EProjectFolderType::Content,		"content" },
		{ Scion::Core::EProjectFolderType::Scripts,		"content/scripts" },
		// Asset Folders
		{ Scion::Core::EProjectFolderType::Assets,		"content/assets" },
		{ Scion::Core::EProjectFolderType::SoundFx,		"content/assets/soundfx" },
		{ Scion::Core::EProjectFolderType::Music,		"content/assets/music" },
		{ Scion::Core::EProjectFolderType::Textures,		"content/assets/textures" },
		{ Scion::Core::EProjectFolderType::Shaders,		"content/assets/shaders" },
		{ Scion::Core::EProjectFolderType::Fonts,		"content/assets/fonts" },
		{ Scion::Core::EProjectFolderType::Prefabs,		"content/assets/prefabs" },
		{ Scion::Core::EProjectFolderType::Scenes,		"content/assets/scenes" },
		// Config Folders
		{ Scion::Core::EProjectFolderType::Config,		"config"},
		{ Scion::Core::EProjectFolderType::GameConfig,	"config/game"},
		{ Scion::Core::EProjectFolderType::EditorConfig, "config/editor"},
	};
// clang-format on

namespace Scion::Editor
{

bool ProjectLoader::CreateNewProject( const std::string& sProjectName, const std::string& sFilepath )
{
	auto& pProjectInfo = MAIN_REGISTRY().GetContext<Scion::Core::ProjectInfoPtr>();
	SCION_ASSERT( pProjectInfo && "Project Info must exist." );

	// Create the game filepath
	fs::path gameFilepath{
		fmt::format( "{}{}{}{}{}", sFilepath, PATH_SEPARATOR, sProjectName, PATH_SEPARATOR, "SCION_2D" ) };

	if ( fs::is_directory( gameFilepath ) )
	{
		SCION_ERROR( "Project [{}] at [{}] already exists.", sProjectName, sFilepath );
		return false;
	}

	std::error_code ec;
	for ( const auto& [ eFolderType, sSubDir ] : mapProjectDirs )
	{
		fs::path fullpath{ gameFilepath / fs::path{ sSubDir } };

		if ( !fs::create_directories( fullpath, ec ) )
		{
			SCION_ERROR( "Failed to create directories: {} - {}", fullpath.string(), ec.message() );
			return false;
		}

		pProjectInfo->AddFolderPath( eFolderType, fullpath );
	}

	pProjectInfo->SetProjectName( sProjectName );
	pProjectInfo->SetProjectPath( gameFilepath );

	return CreateProjectFile( sProjectName, gameFilepath.string() );
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
	auto& pProjectInfo = mainRegistry.GetContext<Scion::Core::ProjectInfoPtr>();
	SCION_ASSERT( pProjectInfo && "Project Info must be valid!" );

	// We need the project filepath saved!
	pProjectInfo->SetProjectFilePath( sFilepath );

	const rapidjson::Value& projectData = doc[ "project_data" ];

	// Set the project name. The actual project name might be different to the project files name.
	pProjectInfo->SetProjectName( projectData[ "project_name" ].GetString() );

	// We need to load all the assets
	if ( !projectData.HasMember( "assets" ) )
	{
		SCION_ERROR( "Failed to load project: File [{}] - Expecting \"assets\" member in project file.", sFilepath );
		return false;
	}

	// Get Content Path
	fs::path filePath{ sFilepath };
	fs::path projectPath{ filePath.parent_path() };
	pProjectInfo->SetProjectPath( projectPath );

	// Setup Project Folders
	std::error_code ec;
	for ( const auto& [ eFolderType, subDir ] : mapProjectDirs )
	{
		fs::path fullPath{ projectPath / subDir };
		if ( !fs::exists( fullPath, ec ) )
		{
			SCION_ERROR( "Failed to load project: Failed to setup project folders. [{}] - {}", ec.message() );
			return false;
		}

		if ( !pProjectInfo->AddFolderPath( eFolderType, fullPath ) )
		{
			SCION_ERROR( "Failed to setup project folder [{}]", fullPath.string() );
			return false;
		}
	}

	// Setup Project Files
	if ( auto optScriptPath = pProjectInfo->TryGetFolderPath( Scion::Core::EProjectFolderType::Scripts ) )
	{
		fs::path mainScriptPath = *optScriptPath / "main.lua";
		if ( !fs::exists( mainScriptPath ) )
		{
			SCION_ERROR( "Failed to load project: main.lua file was not found at [{}]", mainScriptPath.string() );
			return false;
		}
	}

	// Setup Project Files
	if ( auto optGameConfigPath = pProjectInfo->TryGetFolderPath( Scion::Core::EProjectFolderType::GameConfig ) )
	{
		fs::path scriptListPath = *optGameConfigPath / "script_list.lua";
		if ( !fs::exists( scriptListPath ) )
		{
			SCION_ERROR( "Failed to load project: script_list.lua file was not found at [{}]",
						 scriptListPath.string() );
			return false;
		}
	}

	// Get the project path before we adjust it to the content path
	CORE_GLOBALS().SetProjectPath( projectPath.string() );

	auto optContentFolderPath = pProjectInfo->TryGetFolderPath( Scion::Core::EProjectFolderType::Content );
	SCION_ASSERT( optContentFolderPath && "Content folder not set correctly in project info." );

	// Check to see if there is a main lua path
	if ( projectData.HasMember( "main_lua_script" ) )
	{
		auto mainLuaScript = *optContentFolderPath / projectData[ "main_lua_script" ].GetString();
		if ( !fs::exists( mainLuaScript ) )
		{
			SCION_ERROR( "Failed to set main lua script path. [{}] does not exist.", mainLuaScript.string() );
			return false;
		}

		pProjectInfo->SetMainLuaScriptPath( mainLuaScript );
	}

	// Check to see if there is a file icon and load it
	if ( projectData.HasMember( "file_icon" ) )
	{
		std::string sFileIcon = projectData[ "file_icon" ].GetString();
		if ( !sFileIcon.empty() )
		{
			auto fileIconPath = *optContentFolderPath / sFileIcon;
			if ( !fs::exists( fileIconPath ) )
			{
				SCION_ERROR( "Failed to set game file icon path. [{}] does not exist.", fileIconPath.string() );
				return false;
			}

			pProjectInfo->SetFileIconPath( fileIconPath );
		}
	}

	auto optGameConfigPath = pProjectInfo->TryGetFolderPath( Scion::Core::EProjectFolderType::GameConfig );
	SCION_ASSERT( optGameConfigPath && "Game Config folder path has not been setup correctly in project info." );

	fs::path scriptListPath = *optGameConfigPath / "script_list.lua";
	if ( !fs::exists( scriptListPath ) )
	{
		SCION_ERROR( "Failed to load project. ScriptList was not found at path [{}]", scriptListPath.string() );
		return false;
	}

	pProjectInfo->SetScriptListPath( scriptListPath );

	auto& coreGlobals = CORE_GLOBALS();

	if ( projectData.HasMember( "game_type" ) )
	{
		coreGlobals.SetGameType( coreGlobals.GetGameTypeFromStr( projectData[ "game_type" ].GetString() ) );
	}

	if ( projectData.HasMember( "copyright" ) )
	{
		std::string sCopyRight = projectData[ "copyright" ].GetString();
		pProjectInfo->SetCopyRightNotice( sCopyRight );
	}

	if ( projectData.HasMember( "version" ) )
	{
		std::string sVersion = projectData[ "version" ].GetString();
		pProjectInfo->SetProjectVersion( sVersion );
	}

	if ( projectData.HasMember( "description" ) )
	{
		std::string sDescription = projectData[ "description" ].GetString();
		pProjectInfo->SetProjectDescription( sDescription );
	}

	if ( projectData.HasMember( "default_scene" ) )
	{
		std::string sDefaultScene = projectData[ "default_scene" ].GetString();
		pProjectInfo->SetDefaultScene( sDefaultScene );
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
			std::string sJsonTexturePath = jsonTexture[ "path" ].GetString();
			fs::path texturePath = *optContentFolderPath / sJsonTexturePath;

			if ( !assetManager.AddTexture( sTextureName,
										   texturePath.string(),
										   jsonTexture[ "bPixelArt" ].GetBool(),
										   jsonTexture[ "bTilemap" ].GetBool() ) )
			{
				SCION_ERROR( "Failed to load texture [{}] at path [{}]", sTextureName, texturePath.string() );
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
			std::string sJsonSoundFxPath = jsonSoundFx[ "path" ].GetString();
			fs::path soundFxPath = *optContentFolderPath / sJsonSoundFxPath;

			if ( !assetManager.AddSoundFx( sSoundFxName, soundFxPath.string() ) )
			{
				SCION_ERROR( "Failed to load soundfx [{}] at path [{}]", sSoundFxName, soundFxPath.string() );
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
			std::string sJsonMusicPath = jsonMusic[ "path" ].GetString();
			fs::path musicPath = *optContentFolderPath / sJsonMusicPath;

			if ( !assetManager.AddMusic( sMusicName, musicPath.string() ) )
			{
				SCION_ERROR( "Failed to load music [{}] at path [{}]", sMusicName, musicPath.string() );
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
			std::string sJsonFontPath = jsonFonts[ "path" ].GetString();
			fs::path fontPath = *optContentFolderPath / sJsonFontPath;

			if ( !assetManager.AddFont( sFontName, fontPath.string(), jsonFonts[ "fontSize" ].GetFloat() ) )
			{
				SCION_ERROR( "Failed to load fonts [{}] at path [{}]", sFontName, fontPath.string() );
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
			std::string sJsonScenePath = jsonScenes[ "sceneData" ].GetString();
			fs::path scenePath = *optContentFolderPath / sJsonScenePath;

			if ( !sceneManager.AddSceneObject( sSceneName, scenePath.string() ) )
			{
				SCION_ERROR( "Failed to load scene [{}] at path [{}]", sSceneName, scenePath.string() );
			}
		}
	}

	// Load all prefabs to the scene manager
	if ( assets.HasMember( "prefabs" ) )
	{
		const rapidjson::Value& prefabs = assets[ "prefabs" ];

		if ( !prefabs.IsArray() )
		{
			SCION_ERROR( "Failed to load project: File [{}] - Expecting \"prefabs\" must be an array.", sFilepath );
			return false;
		}

		for ( const auto& jsonPrefab : prefabs.GetArray() )
		{
			std::string sName{ jsonPrefab[ "name" ].GetString() };
			std::string sJsonPrefabPath = jsonPrefab[ "path" ].GetString();
			fs::path prefabPath = *optContentFolderPath / sJsonPrefabPath;

			if ( auto pPrefab = Scion::Core::PrefabCreator::CreatePrefab( prefabPath.string() ) )
			{
				if ( !assetManager.AddPrefab( sName, std::move( pPrefab ) ) )
				{
					SCION_ERROR( "Failed to load scene: {}", sName );
				}
			}
			else
			{
				SCION_ERROR( "Failed to load prefab [{}] from path [{}].", sName, prefabPath.string() );
			}
		}
	}

	if ( !pProjectInfo->GetDefaultScene().empty() )
	{
		SCENE_MANAGER().SetCurrentScene( pProjectInfo->GetDefaultScene() );
		if ( !SCENE_MANAGER().LoadCurrentScene() )
		{
			SCION_ERROR( "Failed to load default scene [{}]", pProjectInfo->GetDefaultScene() );
		}
	}

	if ( projectData.HasMember( "physics" ) )
	{
		const rapidjson::Value& physics = projectData[ "physics" ];
		bool bEnabled = physics[ "enabled" ].GetBool();
		if ( bEnabled )
		{
			coreGlobals.EnablePhysics();
		}
		else
		{
			coreGlobals.DisablePhysics();
		}

		coreGlobals.SetGravity( physics[ "gravity" ].GetFloat() );
		coreGlobals.SetVelocityIterations( physics[ "velocityIterations" ].GetInt() );
		coreGlobals.SetPositionIterations( physics[ "positionIterations" ].GetInt() );
	}

	if (projectData.HasMember("audio_config"))
	{
		auto& audioConfig = pProjectInfo->GetAudioConfig();
		const rapidjson::Value& jsonAudioConfig = projectData[ "audio_config" ];
		audioConfig.bGlobalOverrideEnabled = jsonAudioConfig[ "bGlobalEnabled" ].GetBool();
		audioConfig.globalVolumeOverride = jsonAudioConfig[ "globalVolume" ].GetInt();
		audioConfig.bMusicVolumeOverrideEnabled = jsonAudioConfig[ "bMusicOverrideEnabled" ].GetBool();
		audioConfig.musicVolumeOverride = jsonAudioConfig[ "musicVolume" ].GetInt();
		int allocatedChannels = jsonAudioConfig[ "allocatedChannels" ].GetInt();
		int channelDelta = allocatedChannels - audioConfig.GetAllocatedChannelCount();
		if (channelDelta > 0)
		{
			audioConfig.UpdateSoundChannels(channelDelta);
		}

		if (jsonAudioConfig.HasMember("sound_channel_data") && jsonAudioConfig["sound_channel_data"].IsArray())
		{
			const rapidjson::Value& jsonChannelDataArray = jsonAudioConfig[ "sound_channel_data" ];
			for (const auto& channelData : jsonChannelDataArray.GetArray())
			{
				int channelID = channelData[ "channelID" ].GetInt();
				bool bEnabled = channelData[ "bEnabled" ].GetBool();
				int volume = channelData[ "volume" ].GetInt();

				if (!audioConfig.EnableChannelOverride(channelID, bEnabled))
				{
					SCION_ERROR( "Failed to enable sound channel override. Channel [{}] is invalid.", channelID );
				}

				if (!audioConfig.SetChannelVolume(channelID, volume))
				{
					SCION_ERROR( "Failed to set sound channel volume override. Channel [{}] is invalid.", channelID );
				}
			}
		}
	}

	auto& pEditorState = mainRegistry.GetContext<EditorStatePtr>();
	if ( !pEditorState->Load( *pProjectInfo ) )
	{
		SCION_ERROR( "Failed to load editor state." );
		return false;
	}

	return true;
}

bool ProjectLoader::SaveLoadedProject( const Scion::Core::ProjectInfo& projectInfo )
{
	auto optProjectFilePath = projectInfo.GetProjectFilePath();
	SCION_ASSERT( optProjectFilePath && "Project file path not set correctly." );

	if ( !fs::exists( *optProjectFilePath ) )
	{
		SCION_ERROR( "Failed to save project file for [{}] at path [{}]",
					 projectInfo.GetProjectName(),
					 optProjectFilePath->string() );
		return false;
	}

	std::unique_ptr<JSONSerializer> pSerializer{ nullptr };

	try
	{
		pSerializer = std::make_unique<JSONSerializer>( optProjectFilePath->string() );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to save tilemap [{}] - [{}]", optProjectFilePath->string(), ex.what() );
		return false;
	}

	auto& assetManager = ASSET_MANAGER();
	auto& sceneMananger = SCENE_MANAGER();

	if ( !sceneMananger.SaveAllScenes() )
	{
		SCION_ERROR( "Failed to save all scenes." );
	}
	auto& coreGlobals = CORE_GLOBALS();
	pSerializer->StartDocument();
	pSerializer->StartNewObject( "warnings" );
	pSerializer->AddKeyValuePair( "warning", std::string{ "THIS FILE IS ENGINE GENERATED." } )
		.AddKeyValuePair( "warning", std::string{ "DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING." } );
	pSerializer->EndObject(); // Warnings

	auto optMainLuaScript = projectInfo.GetMainLuaScriptPath();
	SCION_ASSERT( optMainLuaScript && "Main Lua script not setup correctly in project info." );
	std::string sMainLuaScript = optMainLuaScript->string();

	std::string sFileIconPath{};
	auto optGameFileIcon = projectInfo.GetFileIconPath();
	if ( optGameFileIcon )
	{
		std::string sGameFileIcon{ optGameFileIcon->string() };
		sFileIconPath = sGameFileIcon.substr( sGameFileIcon.find( CONTENT_FOLDER ) + CONTENT_FOLDER.size() + 1 );
	}

	pSerializer->StartNewObject( "project_data" )
		.AddKeyValuePair( "project_name", projectInfo.GetProjectName() )
		.AddKeyValuePair( "main_lua_script", sMainLuaScript.substr( sMainLuaScript.find( SCRIPTS ) ) )
		.AddKeyValuePair( "file_icon", sFileIconPath )
		.AddKeyValuePair( "game_type", coreGlobals.GetGameTypeStr( coreGlobals.GetGameType() ) )
		.AddKeyValuePair( "copyright", projectInfo.GetCopyRightNotice() )
		.AddKeyValuePair( "version", projectInfo.GetProjectVersion() )
		.AddKeyValuePair( "description", projectInfo.GetProjectDescription() )
		.AddKeyValuePair( "default_scene", projectInfo.GetDefaultScene() )
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
			.AddKeyValuePair( "bPixelArt", pTexture->GetType() == Scion::Rendering::Texture::TextureType::PIXEL )
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

	pSerializer->StartNewArray( "fonts" );
	for ( const auto& [ sName, pFont ] : assetManager.GetAllFonts() )
	{
		std::string sFilename{ pFont->GetFilename() };
		// If the filename is empty, it must have been loaded from memory
		if ( sFilename.empty() )
			continue;

		std::string sFontPath = sFilename.substr( sFilename.find( ASSETS ) );
		pSerializer->StartNewObject()
			.AddKeyValuePair( "name", sName )
			.AddKeyValuePair( "path", sFontPath )
			.AddKeyValuePair( "fontSize", pFont->GetFontSize() )
			.EndObject();
	}
	pSerializer->EndArray(); // Fonts

	pSerializer->StartNewArray( "scenes" );

	for ( const auto& [ sName, pScene ] : sceneMananger.GetAllScenes() )
	{
		std::string sScenePath = pScene->GetSceneDataPath().substr( pScene->GetSceneDataPath().find( ASSETS ) );
		pSerializer->StartNewObject()
			.AddKeyValuePair( "name", sName )
			.AddKeyValuePair( "sceneData", sScenePath )
			.EndObject();
	}
	pSerializer->EndArray(); // Scenes

	pSerializer->StartNewArray( "prefabs" );

	for ( const auto& [ sName, pPrefab ] : assetManager.GetAllPrefabs() )
	{
		std::string sFilepath = pPrefab->GetFilepath().substr( pPrefab->GetFilepath().find( ASSETS ) );
		pSerializer->StartNewObject().AddKeyValuePair( "name", sName ).AddKeyValuePair( "path", sFilepath ).EndObject();
	}

	pSerializer->EndArray(); // Prefabs

	pSerializer->EndObject(); // Assets
	pSerializer->StartNewObject( "physics" )
		.AddKeyValuePair( "enabled", coreGlobals.IsPhysicsEnabled() )
		.AddKeyValuePair( "gravity", coreGlobals.GetGravity() )
		.AddKeyValuePair( "velocityIterations", coreGlobals.GetVelocityIterations() )
		.AddKeyValuePair( "positionIterations", coreGlobals.GetPositionIterations() )
		.EndObject(); // Physics

	const auto& audioConfig = projectInfo.GetAudioConfig();

	pSerializer->StartNewObject( "audio_config" )
		.AddKeyValuePair( "bGlobalEnabled", audioConfig.bGlobalOverrideEnabled )
		.AddKeyValuePair( "globalVolume", audioConfig.globalVolumeOverride )
		.AddKeyValuePair( "bMusicOverrideEnabled", audioConfig.bMusicVolumeOverrideEnabled )
		.AddKeyValuePair( "musicVolume", audioConfig.musicVolumeOverride )
		.AddKeyValuePair( "allocatedChannels", audioConfig.GetAllocatedChannelCount() );

	pSerializer->StartNewArray( "sound_channel_data" );

	for ( const auto& [ channelID, state ] : audioConfig.GetSoundChannelMap() )
	{
		pSerializer->StartNewObject()
			.AddKeyValuePair( "channelID", channelID )
			.AddKeyValuePair( "bEnabled", state.first )
			.AddKeyValuePair( "volume", state.second )
			.EndObject();
	}

	pSerializer
		->EndArray()  // Sound Channel Data
		.EndObject(); // Audio Config

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

	if ( !CreateScriptListFile() )
	{
		SCION_ERROR( "Failed to create Script List" );
		false;
	}

	std::string sProjectFile{ sFilepath + PATH_SEPARATOR + sProjectName + std::string{ S2D_PRJ_FILE_EXT } };

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
	auto& pProjectInfo = MAIN_REGISTRY().GetContext<Scion::Core::ProjectInfoPtr>();
	pProjectInfo->SetProjectFilePath( fs::path{ sProjectFile } );

	auto optMainLuaScript = pProjectInfo->GetMainLuaScriptPath();
	SCION_ASSERT( optMainLuaScript && "Main lua script not setup in project info." );
	std::string sMainLuaScript = optMainLuaScript->string();

	pSerializer->StartDocument();
	pSerializer->StartNewObject( "warnings" );
	pSerializer->AddKeyValuePair( "warning", std::string{ "THIS FILE IS ENGINE GENERATED." } )
		.AddKeyValuePair( "warning", std::string{ "DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING." } );
	pSerializer->EndObject(); // Warnings

	pSerializer->StartNewObject( "project_data" )
		.AddKeyValuePair( "project_name", sProjectName )
		.AddKeyValuePair( "main_lua_file", sMainLuaScript.substr( sMainLuaScript.find( SCRIPTS ) ) )
		.AddKeyValuePair( "game_type", std::string{ "No Type" } )
		.StartNewObject( "assets" )
		.StartNewArray( "textures" )
		.EndArray() // Textures
		.StartNewArray( "soundfx" )
		.EndArray() // SoundFx
		.StartNewArray( "music" )
		.EndArray() // Music
		.StartNewArray( "scenes" )
		.EndArray()	 // Scenes
		.EndObject() // Assets
		.StartNewObject( "physics" )
		.AddKeyValuePair( "enabled", true )
		.AddKeyValuePair( "gravity", 9.8f )
		.AddKeyValuePair( "velocityIterations", 8 )
		.AddKeyValuePair( "positionIterations", 10 )
		.EndObject();		  // Physics
	pSerializer->EndObject(); // Project Data

	return pSerializer->EndDocument();
}

bool ProjectLoader::CreateMainLuaScript( const std::string& sProjectName, const std::string& sFilepath )
{
	fs::path mainLuaFilePath{ sFilepath };
	mainLuaFilePath /= "content";
	mainLuaFilePath /= "scripts";
	mainLuaFilePath /= "main.lua";

	auto pLuaSerializer = std::make_unique<LuaSerializer>( mainLuaFilePath.string() );
	SCION_ASSERT( pLuaSerializer );

	// Save the main lua file path
	MAIN_REGISTRY().GetContext<Scion::Core::ProjectInfoPtr>()->SetMainLuaScriptPath( mainLuaFilePath );

	pLuaSerializer->AddBlockComment( "\tMain Lua script. This is needed to run all scripts in the editor"
									 "\n\tGENERATED BY THE ENGINE ON PROJECT CREATION. DON'T CHANGE UNLESS "
									 "\n\tYOU KNOW WHAT YOU ARE DOING!" );

	pLuaSerializer->AddComment( "The engine looks for these three functions." )
		.AddComment( "Please add your code inside of the init, update, and render functions as needed." );

	pLuaSerializer->StartNewTable( "main" )
		.StartNewTable( "1", true, true )
		.AddKeyValuePair( "init", "function() --[[ Initialize global state variables here ]] end", true, true )
		.EndTable() // init function
		.StartNewTable( "2", true, true )
		.AddKeyValuePair( "update", "function() --[[ Main Lua Update function ]] end", true, true )
		.EndTable() // update function
		.StartNewTable( "3", true, true )
		.AddKeyValuePair( "render", "function() --[[ Main Lua Render function ]] end", true, true )
		.EndTable()	 // render function
		.EndTable(); // main table

	return pLuaSerializer->FinishStream();
}

bool ProjectLoader::CreateScriptListFile()
{
	auto& pProjectInfo = MAIN_REGISTRY().GetContext<Scion::Core::ProjectInfoPtr>();

	auto optPath = pProjectInfo->TryGetFolderPath( Scion::Core::EProjectFolderType::GameConfig );
	if ( !optPath )
	{
		SCION_ERROR( "Failed to create script list file. Game Config path does not exist." );
		return false;
	}

	fs::path scriptListPath = *optPath / "script_list.lua";

	if ( !fs::exists( scriptListPath ) )
	{
		std::ofstream file{ scriptListPath.string() };
		file.close();
	}

	std::error_code ec;
	if ( !fs::exists( scriptListPath, ec ) )
	{
		SCION_ERROR( "Failed to create script list: {}", ec.message() );
		return false;
	}

	pProjectInfo->SetScriptListPath( scriptListPath );

	return true;
}

} // namespace Scion::Editor
