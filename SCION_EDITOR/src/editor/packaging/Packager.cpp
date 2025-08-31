#include "Packager.h"
#include "ScriptCompiler.h"
#include "IconReplacer.h"
#include "AssetPackager.h"

#include "editor/scene/SceneObject.h"
#include "ScionFilesystem/Serializers/LuaSerializer.h"
#include "ScionUtilities/HelperUtilities.h"
#include "ScionUtilities/ThreadPool.h"

#include "Core/CoreUtilities/ProjectInfo.h"
#include "Logger/Logger.h"
#include <rapidjson/error/en.h>

namespace fs = std::filesystem;
using namespace std::chrono_literals;
using namespace SCION_FILESYSTEM;
using namespace SCION_CORE;

// clang-format off
#ifdef _DEBUG
constexpr std::array<std::string_view, 16> CopyPackageFiles = {
	"fmtd.dll",
	"lua.dll",
	"ogg.dll",
	"SCION_CRASH_REPORTER.exe",
	"SCION_ENGINE.exe",
	"SDL2_imaged.dll",
	"SDL2_mixerd.dll",
	"SDL2d.dll",
	"vorbis.dll",
	"vorbisfile.dll",
	"wavpackdll.dll",
	"zlibd1.dll",
	"bz2d.dll",
	"zip.dll",
	"libzippp.dll",
	"mpg123.dll"
};
#else
constexpr std::array<std::string_view, 16> CopyPackageFiles = {
	"fmt.dll",
	"lua.dll",
	"ogg.dll",
	"SCION_CRASH_REPORTER.exe",
	"SCION_ENGINE.exe",
	"SDL2_image.dll",
	"SDL2_mixer.dll",
	"SDL2.dll",
	"vorbis.dll",
	"vorbisfile.dll",
	"wavpackdll.dll",
	"zlib.dll",
	"bz2.dll",
	"zip.dll",
	"libzippp.dll",
	"mpg123.dll"
};
#endif

// clang-format on

namespace SCION_EDITOR
{
Packager::Packager( std::unique_ptr<PackageData> pData, std::shared_ptr<SCION_UTIL::ThreadPool> pThreadPool )
	: m_pPackageData{ std::move( pData ) }
	, m_bPackaging{ false }
	, m_bHasError{ false }
	, m_pThreadPool{ pThreadPool }
{
	m_PackageThread = std::thread( [ this ] { RunPackager(); } );
}

Packager::~Packager()
{
	FinishPackaging();
}

bool Packager::Completed() const
{
	return !m_bPackaging;
}

bool Packager::HasError() const
{
	return m_bHasError;
}
PackagingProgress Packager::GetProgress() const
{
	std::shared_lock lock( m_ProgressMutex );
	return m_Progress;
}

void Packager::FinishPackaging()
{
	// Delete temp files
	try
	{
		if ( fs::exists( m_pPackageData->sTempDataPath ) )
		{
			auto numFiles = fs::remove_all( fs::path{ m_pPackageData->sTempDataPath } );
			if ( numFiles > 0 )
			{
				SCION_LOG( "Successfully deleted temp packaging files. [{}]", numFiles );
			}
		}
	}
	catch ( const fs::filesystem_error& err )
	{
		SCION_ERROR( "Failed to delete temp data at path [{}] - {}", m_pPackageData->sTempDataPath, err.what() );
	}

	if ( m_PackageThread.joinable() )
		m_PackageThread.join();
}

void Packager::RunPackager()
{
	if ( !m_pPackageData )
	{
		SCION_ERROR( "Failed to run packager." );
		return;
	}

	m_bPackaging = true;

	try
	{
		if ( fs::exists( m_pPackageData->sTempDataPath ) )
		{
			auto numFiles = fs::remove_all( fs::path{ m_pPackageData->sTempDataPath } );
			if ( numFiles > 0 )
			{
				SCION_LOG( "Successfully deleted temp packaging files. [{}]", numFiles );
			}
		}
	}
	catch ( const fs::filesystem_error& err )
	{
		SCION_ERROR(
			"Failed to delete packaging temp data at path: [{}] - {}", m_pPackageData->sTempDataPath, err.what() );
		m_bPackaging = false;
		return;
	}

	try
	{
		UpdateProgress( 0.f, "Starting packaging." );
		if ( fs::exists( fs::path{ m_pPackageData->sFinalDestination } ) )
		{
			SCION_ERROR( "Failed to package game. [{}] directory already exists.", m_pPackageData->sFinalDestination );
			UpdateProgress( 0.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		UpdateProgress( 5.f, "Creating package destination." );
		std::error_code ec;
		if ( !fs::create_directory( fs::path{ m_pPackageData->sFinalDestination }, ec ) )
		{
			SCION_ERROR( "Failed to create directory [{}] - {}", m_pPackageData->sFinalDestination, ec.message() );
			UpdateProgress( 5.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		UpdateProgress( 10.f, "Creating temp data." );
		if ( !fs::exists( fs::path{ m_pPackageData->sTempDataPath } ) )
		{
			if ( !fs::create_directory( m_pPackageData->sTempDataPath ) )
			{
				SCION_ERROR( "Failed to create temporary data path at [{}]", m_pPackageData->sTempDataPath );
				UpdateProgress( 10.f, "Packaging failed. Please see logs." );
				m_bHasError = true;
				return;
			}
		}

		UpdateProgress( 25.f, "Adding game lua scripts." );
		auto pScriptCompiler = std::make_unique<ScriptCompiler>();
		auto optScriptListPath = m_pPackageData->pProjectInfo->GetScriptListPath();
		SCION_ASSERT( optScriptListPath && "Script List Path Must be set." );
		if ( !optScriptListPath )
		{
			SCION_ERROR( "Failed to add scripts. Script list path was not set in the project info." );
			UpdateProgress( 25.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		if ( !pScriptCompiler->AddScripts( optScriptListPath->string() ) )
		{
			SCION_ERROR( "Failed to add scripts." );
			UpdateProgress( 25.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		UpdateProgress( 50.f, "Start asset copying." );

		auto optProjectFilePath = m_pPackageData->pProjectInfo->GetProjectFilePath();
		SCION_ASSERT( optProjectFilePath && "Project File path must be set in project info." );
		if ( !optProjectFilePath )
		{
			SCION_ERROR( "Failed to open project file. Project file path was not set in the project info." );
			UpdateProgress( 50.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		// Now we need to read in the files from the json project file
		std::ifstream procFile;
		procFile.open( optProjectFilePath->string() );

		if ( !procFile.is_open() )
		{
			SCION_ERROR( "Failed to open project file [{}]", optProjectFilePath->string() );
			UpdateProgress( 50.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
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
						 optProjectFilePath->string(),
						 rapidjson::GetParseError_En( doc.GetParseError() ),
						 doc.GetErrorOffset() );
			UpdateProgress( 50.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		// Get the project data
		if ( !doc.HasMember( "project_data" ) )
		{
			SCION_ERROR( "Failed to load project: File [{}] - Expecting \"project_data\" member in project file.",
						 optProjectFilePath->string() );
			UpdateProgress( 50.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		const rapidjson::Value& projectData = doc[ "project_data" ];

		// We need to load all the assets
		if ( !projectData.HasMember( "assets" ) )
		{
			SCION_ERROR( "Failed to load project: File [{}] - Expecting \"assets\" member in project file.",
						 optProjectFilePath->string() );
			UpdateProgress( 50.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		const rapidjson::Value& assets = projectData[ "assets" ];

		if ( m_pPackageData->pGameConfig->bPackageAssets )
		{
			UpdateProgress( 60.f, "Starting packaging of assets." );
			AssetPackagerParams assetPackagerParams{
				.sTempFilepath = m_pPackageData->sTempDataPath,
				.sDestinationPath = m_pPackageData->sFinalDestination + PATH_SEPARATOR + "assets",
				.sProjectPath = m_pPackageData->pProjectInfo->GetProjectPath().string() };

			AssetPackager assetPackager{ assetPackagerParams, m_pThreadPool };

			assetPackager.PackageAssets( assets );
		}
		else
		{
			UpdateProgress( 60.f, "Creating asset defs luac file." );
			std::string sAssetDefsFile = CreateAssetDefsFile( m_pPackageData->sTempDataPath, assets );
			if ( !fs::exists( sAssetDefsFile ) )
			{
				SCION_ERROR( "Failed to create asset defs file." );
				UpdateProgress( 60.f, "Packaging failed. Please see logs." );
				m_bHasError = true;
				return;
			}

			UpdateProgress( 65.f, "Adding asset defs file to script compiler." );
			if ( !pScriptCompiler->AddScript( sAssetDefsFile ) )
			{
				SCION_ERROR( "Failed to find the assetDefs.lua file at path [{}].", sAssetDefsFile );
				UpdateProgress( 65.f, "Packaging failed. Please see logs." );
				m_bHasError = true;
				return;
			}
		}

		UpdateProgress( 70.f, "Start packaging of all scenes." );
		// We need to load all the scenes
		if ( !assets.HasMember( "scenes" ) )
		{
			SCION_ERROR( "Failed to load project: File [{}] - Expecting \"scenes\" member in project file.",
						 optProjectFilePath->string() );

			UpdateProgress( 70.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		UpdateProgress( 75.f, "Creating scene Files." );
		const rapidjson::Value& scenes = assets[ "scenes" ];
		auto sceneFiles = CreateSceneFiles( m_pPackageData->sTempDataPath, scenes );

		if ( sceneFiles.empty() )
		{
			SCION_ERROR( "Failed to create scene files or scene files are invalid." );
			UpdateProgress( 75.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		for ( const auto& sSceneFile : sceneFiles )
		{
			if ( !pScriptCompiler->AddScript( sSceneFile ) )
			{
				SCION_ERROR( "Failed to add scene file [{}] to scripts to be compiled.", sSceneFile );
				UpdateProgress( 75.f, "Packaging failed. Please see logs." );
				m_bHasError = true;
				return;
			}
		}

		UpdateProgress( 80.f, "Adding main lua script." );

		auto optMainLuaScript = m_pPackageData->pProjectInfo->GetMainLuaScriptPath();
		SCION_ASSERT( optMainLuaScript && "Main Lua script not set in the project info." );
		if ( !optMainLuaScript )
		{
			SCION_ERROR( "Failed to add main.lua script. Path not set in the project info." );
			UpdateProgress( 80.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		if ( !fs::exists( *optMainLuaScript ) )
		{
			SCION_ERROR( "Failed to find the main.lua file at path [{}].", optMainLuaScript->string() );
			UpdateProgress( 80.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		if ( !pScriptCompiler->AddScript( optMainLuaScript->string() ) )
		{
			SCION_ERROR( "Failed to add main.lua script." );
			m_bHasError = true;
			return;
		}

		pScriptCompiler->SetOutputFileName(
			fmt::format( "{}{}master.luac", m_pPackageData->sTempDataPath, PATH_SEPARATOR ) );

		UpdateProgress( 85.f, "Compiling game lua scripts." );
		pScriptCompiler->Compile();

		UpdateProgress( 87.f, "Creating config.lua file." );
		std::string sConfigFile = CreateConfigFile( m_pPackageData->sTempDataPath );

		if ( !fs::exists( sConfigFile ) )
		{
			SCION_ERROR( "Failed to create config file while packaging." );
			UpdateProgress( 87.f, "Packaging failed. Please see logs." );
			m_bHasError = true;
			return;
		}

		pScriptCompiler->ClearScripts();
		pScriptCompiler->AddScript( sConfigFile );
		pScriptCompiler->SetOutputFileName(
			fmt::format( "{}{}config.luac", m_pPackageData->sTempDataPath, PATH_SEPARATOR ) );
		UpdateProgress( 90.f, "Compiling config file." );
		pScriptCompiler->Compile();
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to package game. Error: {}", ex.what() );
		UpdateProgress( 0.f, "Packaging failed. Please see logs." );
		m_bHasError = true;
		return;
	}

	UpdateProgress( 95.f, "Copying necessary files to packaged game destination." );
	CopyFilesToDestination();

	UpdateProgress( 100.f, "Packaging Complete." );
	m_bPackaging = false;
}

void Packager::UpdateProgress( float percent, std::string_view message )
{
	std::lock_guard lock( m_ProgressMutex );
	m_Progress.percent = percent;
	m_Progress.sMessage = message;
}

std::string Packager::CreateConfigFile( const std::string& sTempFilepath )
{
	std::unique_ptr<LuaSerializer> pSerializer{ nullptr };
	if ( !fs::exists( sTempFilepath ) )
	{
		SCION_ERROR( "Failed to create settings config file. [{}] path does not exist.", sTempFilepath );
		return {};
	}

	const std::string sSettingsConfigPath = sTempFilepath + PATH_SEPARATOR + "config.lua";

	try
	{
		pSerializer = std::make_unique<LuaSerializer>( sSettingsConfigPath );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to create settings config file [{}]. Error: {}", sSettingsConfigPath, ex.what() );
		return {};
	}

	pSerializer->StartNewTable( "GameConfig" )
		.AddKeyValuePair( "GameName", m_pPackageData->pGameConfig->sGameName, true, false, false, true )
		.AddKeyValuePair( "StartupScene", m_pPackageData->pGameConfig->sStartupScene, true, false, false, true )
		.AddKeyValuePair( "bPackageAssets", m_pPackageData->pGameConfig->bPackageAssets ? "true" : "false" )
		.StartNewTable( "WindowParams" )
		.AddKeyValuePair( "width", m_pPackageData->pGameConfig->windowWidth )
		.AddKeyValuePair( "height", m_pPackageData->pGameConfig->windowHeight )
		.AddKeyValuePair( "flags", m_pPackageData->pGameConfig->windowFlags )
		.EndTable() // WindowParams
		.StartNewTable( "PhysicsParams" )
		.AddKeyValuePair( "bEnabled", m_pPackageData->pGameConfig->bPhysicsEnabled ? "true" : "false" )
		.AddKeyValuePair( "positionIterations", m_pPackageData->pGameConfig->positionIterations )
		.AddKeyValuePair( "velocityIterations", m_pPackageData->pGameConfig->velocityIterations )
		.AddKeyValuePair( "gravity", m_pPackageData->pGameConfig->gravity )
		.EndTable() // PhysicsParams
		.EndTable() // GameConfig
		.FinishStream();

	return sSettingsConfigPath;
}

std::string Packager::CreateAssetDefsFile( const std::string& sTempFilepath, const rapidjson::Value& assets )
{
	std::unique_ptr<LuaSerializer> pSerializer{ nullptr };
	if ( !fs::exists( sTempFilepath ) )
	{
		SCION_ERROR( "Failed to create settings config file. [{}] path does not exist.", sTempFilepath );
		return {};
	}

	const std::string sAssetsDefPath = sTempFilepath + PATH_SEPARATOR + "assetDefs.lua";

	auto optProjectFilePath = m_pPackageData->pProjectInfo->GetProjectFilePath();
	SCION_ASSERT( optProjectFilePath && "Project file path must be set in project info." );
	if ( !optProjectFilePath )
	{
		SCION_ERROR( "Failed to create asset defs file. Project file path was not set in project info." );
		return {};
	}

	try
	{
		pSerializer = std::make_unique<LuaSerializer>( sAssetsDefPath );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to create asset defs file [{}]. Error: {}", sAssetsDefPath, ex.what() );
		return {};
	}

	pSerializer->StartNewTable( "Assets" );
	pSerializer->StartNewTable( "textures" );
	if ( assets.HasMember( "textures" ) )
	{
		const rapidjson::Value& textures = assets[ "textures" ];

		if ( !textures.IsArray() )
		{
			SCION_ERROR( "Failed to create asset defs file. Project File [{}]. Expecting \"textures\" as an array.",
						 optProjectFilePath->string() );
			return {};
		}

		for ( const auto& jsonTexture : textures.GetArray() )
		{
			std::string sTextureName{ jsonTexture[ "name" ].GetString() };
			std::string sTexturePath{ jsonTexture[ "path" ].GetString() };
			bool bPixelArt = jsonTexture[ "bPixelArt" ].GetBool();

			pSerializer->StartNewTable()
				.AddKeyValuePair( "name", sTextureName, true, false, false, true )
				.AddKeyValuePair( "path", sTexturePath, true, false, false, true )
				.AddKeyValuePair( "bPixelArt", bPixelArt ? "true" : "false" )
				.EndTable(); // End Current Texture
		}
	}

	pSerializer->EndTable(); // End Textures

	pSerializer->StartNewTable( "soundfx" );
	if ( assets.HasMember( "soundfx" ) )
	{
		const rapidjson::Value& soundfx = assets[ "soundfx" ];

		if ( !soundfx.IsArray() )
		{
			SCION_ERROR( "Failed to create asset defs file. Project File [{}]. Expecting \"soundfx\" as an array.",
						 optProjectFilePath->string() );
			return {};
		}

		for ( const auto& jsonSoundfx : soundfx.GetArray() )
		{
			std::string sSoundFxName{ jsonSoundfx[ "name" ].GetString() };
			std::string sSoundFxPath{ jsonSoundfx[ "path" ].GetString() };

			pSerializer->StartNewTable()
				.AddKeyValuePair( "name", sSoundFxName, true, false, false, true )
				.AddKeyValuePair( "path", sSoundFxPath, true, false, false, true )
				.EndTable(); // End Current SoundFx
		}
	}

	pSerializer->EndTable(); // End Soundfx

	pSerializer->StartNewTable( "music" );
	if ( assets.HasMember( "music" ) )
	{
		const rapidjson::Value& music = assets[ "music" ];

		if ( !music.IsArray() )
		{
			SCION_ERROR( "Failed to create asset defs file. Project File [{}]. Expecting \"music\" as an array.",
						 optProjectFilePath->string() );
			return {};
		}

		for ( const auto& jsonMusic : music.GetArray() )
		{
			std::string sMusicName{ jsonMusic[ "name" ].GetString() };
			std::string sMusicPath{ jsonMusic[ "path" ].GetString() };

			pSerializer->StartNewTable()
				.AddKeyValuePair( "name", sMusicName, true, false, false, true )
				.AddKeyValuePair( "path", sMusicPath, true, false, false, true )
				.EndTable(); // End Current Music
		}
	}

	pSerializer->EndTable(); // End Music

	pSerializer->StartNewTable( "fonts" );
	if ( assets.HasMember( "fonts" ) )
	{
		const rapidjson::Value& fonts = assets[ "fonts" ];

		if ( !fonts.IsArray() )
		{
			SCION_ERROR( "Failed to create asset defs file. Project File [{}]. Expecting \"fonts\" as an array.",
						 optProjectFilePath->string() );
			return {};
		}

		for ( const auto& jsonFonts : fonts.GetArray() )
		{
			std::string sFontName{ jsonFonts[ "name" ].GetString() };
			std::string sFontPath{ jsonFonts[ "path" ].GetString() };

			pSerializer->StartNewTable()
				.AddKeyValuePair( "name", sFontName, true, false, false, true )
				.AddKeyValuePair( "path", sFontPath, true, false, false, true )
				.AddKeyValuePair( "fontSize", jsonFonts[ "fontSize" ].GetFloat() )
				.EndTable(); // End Current Font
		}
	}

	pSerializer->EndTable(); // End Fonts
	pSerializer->EndTable(); // End Assets
	pSerializer->FinishStream();

	pSerializer.reset( nullptr );

	std::fstream assetDefs;
	assetDefs.open( sAssetsDefPath, std::ios::app | std::ios::out );
	if ( !assetDefs.is_open() )
	{
		SCION_ERROR( "Failed to append assets file and add function." );
		return {};
	}

	assetDefs << R"(
	function LoadAssets(assets)
		for k, v in pairs(assets) do
			for i = 1, #v do
				if k == "textures" then
					if not AssetManager.addTexture(v[i].name, v[i].path, v[i].bPixelArt ) then
						print("Failed to load texture ["..v[i].name.."]")
					end
				elseif k == "fonts" then
					if not AssetManager.addFont(v[i].name, v[i].path, v[i].fontSize) then
						print("Failed to load font ["..v[i].name.."]")
					end
				elseif k == "soundfx" then
					if not AssetManager.addSoundfx(v[i].name, v[i].path) then
						print("Failed to load soundfx ["..v[i].name.."]")
					end
				elseif k == "music" then
					if not AssetManager.addMusic(v[i].name, v[i].path) then
						print("Failed to load music ["..v[i].name.."]")
					end
				end
			end 	
		end
	end

	LoadAssets(Assets)
)";

	assetDefs.close();

	return sAssetsDefPath;
}

std::vector<std::string> Packager::CreateSceneFiles( const std::string& sTempFilepath, const rapidjson::Value& scenes )
{
	std::vector<std::string> sceneFiles;
	if ( !scenes.IsArray() )
	{
		SCION_ERROR( "Failed to create scene lua files. \"scenes\" was not a valid json array." );
		return sceneFiles;
	}

	auto optContentPath = m_pPackageData->pProjectInfo->TryGetFolderPath( EProjectFolderType::Content );
	SCION_ASSERT( optContentPath && "Content path was not set in project info." );
	if ( !optContentPath )
	{
		SCION_ERROR( "Failed to create scene lua files. Content path not set in project info." );
		return sceneFiles;
	}

	for ( const auto& jsonScene : scenes.GetArray() )
	{
		std::string sSceneName{ jsonScene[ "name" ].GetString() };
		std::string sSceneDataPath{ jsonScene[ "sceneData" ].GetString() };
		fs::path sceneDataPath = *optContentPath / sSceneDataPath;
		SCION_CORE::ECS::Registry registry;
		auto pSceneObject = std::make_unique<SceneObject>( sSceneName, sceneDataPath.string() );
		auto sceneExportFiles =
			pSceneObject->ExportSceneToLua( sSceneName, m_pPackageData->sTempDataPath, registry );

		if ( !sceneExportFiles.IsValid() )
		{
			SCION_ERROR( "Failed to create scene files for scene [{}]", sSceneName );
			return {};
		}

		sceneFiles.push_back( sceneExportFiles.sTilemapFile);
		sceneFiles.push_back( sceneExportFiles.sObjectFile);
		sceneFiles.push_back( sceneExportFiles.sDataFile);
	}

	return sceneFiles;
}

void Packager::CopyFilesToDestination()
{
	fs::path currentPath = fs::current_path();

	SCION_LOG( "Listing files in: {}\n.", currentPath.string() );
	SCION_LOG( "Copy Destination: {}", m_pPackageData->sFinalDestination );

	try
	{
		for ( const auto& entry : fs::directory_iterator( currentPath ) )
		{
			auto it = std::ranges::find( CopyPackageFiles, entry.path().filename().string() );
			if ( it != CopyPackageFiles.end() )
			{
				const auto& path = entry.path();
				if ( fs::is_regular_file( path ) )
				{
					auto dest = fs::path{ m_pPackageData->sFinalDestination } / path.filename();
					fs::copy( path, dest, fs::copy_options::overwrite_existing );
					SCION_LOG(
						"Copied file [{}] to [{}]", path.filename().string(), m_pPackageData->sFinalDestination );
				}
			}
		}

		fs::path destination{ m_pPackageData->sFinalDestination };

		// Copy the temp luac compiled files
		fs::path configPath = destination / "config";
		if ( !fs::exists( configPath ) )
		{
			fs::create_directory( configPath );
		}

		fs::path scriptPath{ destination / std::format( "{}{}{}", "assets", PATH_SEPARATOR, "scripts" ) };
		if ( !fs::exists( scriptPath ) )
		{
			fs::create_directories( scriptPath );
		}

		fs::path tempDataPath{ m_pPackageData->sTempDataPath };
		if ( !fs::exists( tempDataPath ) )
		{
			SCION_ERROR( "Failed to copy files from temp data path." );
		}
		else
		{
			for ( const auto& entry : fs::directory_iterator( tempDataPath ) )
			{
				if ( entry.path().extension() == ".luac" )
				{
					const auto& path = entry.path();
					if ( fs::is_regular_file( path ) )
					{
						// Put the config file into the config folder
						if ( path.filename().string().find( "config" ) != std::string::npos )
						{
							auto dest = configPath / path.filename();
							fs::copy( path, dest, fs::copy_options::overwrite_existing );
							SCION_LOG( "Copied file [{}] to [{}]", path.filename().string(), configPath.string() );
						} // Only copy the master.luac and the config, possible asset luac files should be deleted.
						else if ( path.filename().string().find( "master.luac" ) != std::string::npos )
						{
							auto dest = scriptPath / path.filename();
							fs::copy( path, dest, fs::copy_options::overwrite_existing );
							SCION_LOG( "Copied file [{}] to [{}]", path.filename().string(), scriptPath.string() );
						}
					}
				}
			}
		}

		// Replace the SCION_ENGINE.exe name with the game name and change the icon if available.
		for ( const auto& entry : fs::directory_iterator( destination ) )
		{
			if ( entry.path().filename().string() == "SCION_ENGINE.exe" )
			{
				if ( auto optFileIconPath = m_pPackageData->pProjectInfo->GetFileIconPath() )
				{
					IconReplacer iconReplacer{ optFileIconPath->string(), entry.path().string() };
					if ( !iconReplacer.ReplaceIcon() )
					{
						SCION_ERROR( "Failed to replace icon file with [{}]", optFileIconPath->string() );
					}
				}

				fs::rename( entry.path(), destination / ( m_pPackageData->pGameConfig->sGameName + ".exe" ) );
				break;
			}
		}
	}
	catch ( const fs::filesystem_error& err )
	{
		SCION_ERROR( "Failed to copy files to destination: [{}] - {}", m_pPackageData->sFinalDestination, err.what() );
		return;
	}

	// Only copy the assets directly if not packaged.
	if ( !m_pPackageData->pGameConfig->bPackageAssets )
	{
		CopyAssetsToDestination();
	}
}

void Packager::CopyAssetsToDestination()
{
	auto optAssetPath = m_pPackageData->pProjectInfo->TryGetFolderPath( EProjectFolderType::Assets );
	SCION_ASSERT( optAssetPath && "Assets folder was not set in project info." );

	if ( !optAssetPath )
	{
		SCION_ERROR( "Faield to copy the game assets. Assets folder was not set in project info." );
		return;
	}

	if ( !fs::exists( *optAssetPath ) )
	{
		SCION_ERROR( "Failed to copy the game assets to the destination. " );
		return;
	}
	fs::path destination{ m_pPackageData->sFinalDestination };
	destination /= "assets";

	const std::unordered_set<std::string> foldersToCopy = { "textures", "music", "soundfx", "fonts" };

	for ( fs::recursive_directory_iterator itr( *optAssetPath ), end; itr != end; ++itr )
	{
		const auto& path = itr->path();
		auto relativePath = fs::relative( path, *optAssetPath );

		// Skip directories not in the selected set at the root level
		if ( !relativePath.has_parent_path() && itr->is_directory() )
		{
			if ( !foldersToCopy.contains( path.filename().string() ) )
			{
				itr.disable_recursion_pending();
				continue;
			}
		}

		fs::path destPath = destination / relativePath;

		try
		{
			if ( fs::is_directory( path ) )
			{
				fs::create_directories( destPath );
			}
			else if ( fs::is_regular_file( path ) )
			{
				fs::create_directories( destPath.parent_path() );
				fs::copy_file( path, destPath, fs::copy_options::overwrite_existing );
			}
		}
		catch ( const fs::filesystem_error& err )
		{
			SCION_ERROR( "Failed to copy asset file: {}", err.what() );
		}
	}
}

} // namespace SCION_EDITOR
