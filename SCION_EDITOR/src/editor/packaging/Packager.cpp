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
					if not AssetManager.addSoundFx(v[i].name, v[i].path) then
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
	if (!scenes.IsArray())
	{
		SCION_ERROR( "Failed to create scene lua files. \"scenes\" was not a valid json array." );
		return sceneFiles;
	}

	auto optContentPath = m_pPackageData->pProjectInfo->TryGetFolderPath( EProjectFolderType::Content );
	SCION_ASSERT( optContentPath && "Content path was not set in project info." );
	if (!optContentPath)
	{
		SCION_ERROR( "Failed to create scene lua files. Content path not set in project info." );
		return sceneFiles;
	}

	for (const auto& jsonScene : scenes.GetArray())
	{
		std::string sSceneName{ jsonScene[ "name" ].GetString() };
		std::string sSceneDataPath{ jsonScene[ "sceneData" ].GetString() };
		fs::path sceneDataPath = *optContentPath / sSceneDataPath;
		SCION_CORE::ECS::Registry registry;
		auto pSceneObject = std::make_unique<SceneObject>( sSceneName, sceneDataPath.string() );
		auto [ sTilemap, sObjectMap ] =
			pSceneObject->ExportSceneToLua( sSceneName, m_pPackageData->sTempDataPath, registry );

		if (!fs::exists(fs::path{ sTilemap }) || !fs::exists(fs::path{ sObjectMap }))
		{
			SCION_ERROR( "Failed to create scene files for scene [{}]", sSceneName );
			return {};
		}

		sceneFiles.push_back( sTilemap );
		sceneFiles.push_back( sObjectMap );
	}

	return sceneFiles;
}

void Packager::CopyFilesToDestination()
{
}
void Packager::CopyAssetsToDestination()
{
}
} // namespace SCION_EDITOR
