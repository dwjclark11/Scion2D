#include "AssetPackager.h"
#include "ScionUtilities/ScionUtilities.h"
#include "ScionUtilities/HelperUtilities.h"
#include "ScionUtilities/ThreadPool.h"

#include "ScionFilesystem/Serializers/LuaSerializer.h"
#include "ScriptCompiler.h"
#include "Logger/Logger.h"
#include <libzippp/libzippp.h>

using namespace SCION_FILESYSTEM;
namespace fs = std::filesystem;

namespace SCION_EDITOR
{
AssetPackager::AssetPackager( const AssetPackagerParams& params, std::shared_ptr<SCION_UTIL::ThreadPool> pThreadPool )
	: m_Params{ params }
	, m_pThreadPool{ pThreadPool }
{
}

AssetPackager::~AssetPackager() = default;

void AssetPackager::PackageAssets( const rapidjson::Value& assets )
{
	try
	{
		CreateLuaAssetFiles( m_Params.sProjectPath, assets );

		if ( !CompileLuaAssetFiles() )
		{
			SCION_ERROR( "Failed to compile the assets." );
			return;
		}

		if ( !CreateAssetsZip() )
		{
			SCION_ERROR( "Failed to archive files successfully." );
			return;
		}
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to package assets: {}", ex.what() );
	}
}

void AssetPackager::ConvertAssetToLuaTable( SCION_FILESYSTEM::LuaSerializer& luaSerializer,
											const AssetConversionData& conversionData )
{
	std::fstream in{ conversionData.sInAssetFile, std::ios::in | std::ios::binary };
	if ( !in.is_open() )
		throw std::runtime_error( fmt::format( "Failed to open file [{}].", conversionData.sInAssetFile ) );

	fs::path assetPath{ conversionData.sInAssetFile };

	int readByte{ 0 };
	std::size_t i{ 0U };
	std::size_t count{ 0U };

	try
	{
		luaSerializer.StartNewTable()
			.AddKeyValuePair( "assetName", conversionData.sAssetName, true, false, false, true )
			.AddKeyValuePair( "assetExt", assetPath.extension().string(), true, false, false, true )
			.AddKeyValuePair(
				"assetType", SCION_UTIL::AssetTypeToStr( conversionData.eType ), true, false, false, true );

		if ( conversionData.eType == SCION_UTIL::AssetType::FONT )
		{
			luaSerializer.AddKeyValuePair( "fontSize",
										   conversionData.optFontSize ? *conversionData.optFontSize : 32.f );
		}
		else if ( conversionData.eType == SCION_UTIL::AssetType::TEXTURE )
		{
			luaSerializer.AddKeyValuePair( "bPixelArt",
										   conversionData.optPixelArt ? *conversionData.optPixelArt : true );
		}

		luaSerializer.StartNewTable( "data" );

		while ( ( readByte = in.get() ) != EOF )
		{
			// Keep rows at 100
			if ( count >= 100 )
			{
				luaSerializer.AddWords( ",", true );
				count = 0;
			}

			luaSerializer.AddValue( fmt::format( "{:#04x}", readByte ), false );
			++count;
			++i;
		}

		luaSerializer
			.EndTable() // Data
			.AddKeyValuePair( "dataEnd", i - 1ull )
			.AddKeyValuePair( "dataSize", i )
			.EndTable(); // sAssetNum
	}
	catch ( const std::exception& ex )
	{
		throw std::runtime_error( fmt::format( "Failed to write [{}] at path [{}] to asset file. Error: {}",
											   conversionData.sAssetName,
											   conversionData.sInAssetFile,
											   ex.what() ) );
	}
}

void AssetPackager::CreateLuaAssetFiles( const std::string& sProjectPath, const rapidjson::Value& assets )
{
	if ( !fs::exists( fs::path{ m_Params.sTempFilepath } ) )
	{
		throw std::runtime_error(
			fmt::format( "Failed to create lua asset files. Temp path [{}] does not exist or is invalid.",
						 m_Params.sTempFilepath ) );
	}

	fs::path tempAssetPath{ fmt::format( "{}{}{}", m_Params.sTempFilepath, PATH_SEPARATOR, "assets" ) };
	m_Params.sAssetsPath = tempAssetPath.string();

	if ( !fs::exists( tempAssetPath ) )
	{
		fs::create_directories( tempAssetPath );
	}

	std::string sContentPath = sProjectPath + PATH_SEPARATOR + "content";

	if ( !fs::exists( fs::path{ sContentPath } ) )
	{
		throw std::runtime_error( fmt::format(
			"Failed to create lua asset files. Content path [{}] does not exist or is invalid.", sContentPath ) );
	}

	std::vector<std::future<AssetPackageStatus>> assetFutures;
	assetFutures.emplace_back( m_pThreadPool->Enqueue( [ & ] {
		return SerializeAssetsByType( assets, tempAssetPath, "textures", sContentPath, SCION_UTIL::AssetType::TEXTURE );
	} ) );

	assetFutures.emplace_back( m_pThreadPool->Enqueue( [ & ] {
		return SerializeAssetsByType( assets, tempAssetPath, "soundfx", sContentPath, SCION_UTIL::AssetType::SOUNDFX );
	} ) );

	assetFutures.emplace_back( m_pThreadPool->Enqueue( [ & ] {
		return SerializeAssetsByType( assets, tempAssetPath, "music", sContentPath, SCION_UTIL::AssetType::MUSIC );
	} ) );

	assetFutures.emplace_back( m_pThreadPool->Enqueue( [ & ] {
		return SerializeAssetsByType( assets, tempAssetPath, "fonts", sContentPath, SCION_UTIL::AssetType::FONT );
	} ) );

	bool bHasError{ false };
	std::string sErrorStr{};

	std::ranges::for_each( assetFutures, [ & ]( auto& fut ) {
		try
		{
			auto status = fut.get();
			if ( !status.bSuccess )
			{
				bHasError = true;
				sErrorStr += status.sError + "\n";
			}
		}
		catch ( ... )
		{
			bHasError = true;
			sErrorStr += "Failed to serialize assets. Unknown Error.\n";
		}
	} );

	if ( bHasError )
	{
		throw std::runtime_error( fmt::format( "Failed to serialize assets correctly. {}", sErrorStr ) );
	}
}

bool AssetPackager::CompileLuaAssetFiles()
{
	ScriptCompiler scriptCompiler{};

	for ( const auto& entry : fs::directory_iterator( fs::path{ m_Params.sAssetsPath } ) )
	{
		if ( fs::is_directory( entry ) )
			continue;

		if ( fs::is_regular_file( entry.path() ) && entry.path().extension() == ".s2dasset" )
		{
			if ( !scriptCompiler.AddScript( entry.path().string() ) )
			{
				SCION_ERROR( "Failed to add script: [{}] to asset packager.", entry.path().string() );
				return false;
			}

			scriptCompiler.SetOutputFileName( std::string{
				( fs::path{ m_Params.sTempFilepath } / std::string{ entry.path().stem().string() + ".luac" } )
					.string() } );

			scriptCompiler.Compile();
			scriptCompiler.ClearScripts();
		}
	}

	return true;
}

bool AssetPackager::CreateAssetsZip()
{
	fs::path assetsDestination{ m_Params.sDestinationPath };
	if ( !fs::exists( assetsDestination ) )
	{
		std::error_code ec;
		if ( !fs::create_directories( assetsDestination, ec ) )
		{
			SCION_ERROR( "Failed to create directory [{}]", assetsDestination.string() );
			return false;
		}
	}

	assetsDestination /= "ScionAssets.zip";
	libzippp::ZipArchive zip{ assetsDestination.string() };

	zip.setErrorHandlerCallback(
		[]( const std::string& message, const std::string& strerror, int zip_error_code, int system_error_code ) {
			SCION_ERROR( "Failed to archive assets: Message: [{}] - Error: [{}]", message, strerror );
		} );

	if ( !zip.open( libzippp::ZipArchive::Write ) )
	{
		SCION_ERROR( "Failed to open zip: {}", zip.getPath() );
		return false;
	}

	if ( !zip.addEntry( std::string{ "ScionAssets/" } ) )
	{
		SCION_ERROR( "Failed to add entry to archive." );
		zip.close();
		return false;
	}

	const std::string texturePath{ fmt::format( "{}{}{}", m_Params.sTempFilepath, PATH_SEPARATOR, "textures.luac" ) };
	if ( fs::exists( texturePath ) )
	{
		if ( !zip.addFile( fmt::format( "{}{}{}", "ScionAssets", PATH_SEPARATOR, "textures.luac" ), texturePath ) )
		{
			SCION_ERROR( "Failed to add textures.luac to zip." );
			zip.close();
			return false;
		}
	}

	const std::string musicPath{ fmt::format( "{}{}{}", m_Params.sTempFilepath, PATH_SEPARATOR, "music.luac" ) };
	if ( fs::exists( musicPath ) )
	{
		if ( !zip.addFile( fmt::format( "{}{}{}", "ScionAssets", PATH_SEPARATOR, "music.luac" ), musicPath ) )
		{
			SCION_ERROR( "Failed to add music.luac to zip." );
			zip.close();
			return false;
		}
	}

	const std::string soundFxPath{ fmt::format( "{}{}{}", m_Params.sTempFilepath, PATH_SEPARATOR, "soundfx.luac" ) };
	if ( fs::exists( soundFxPath ) )
	{
		if ( !zip.addFile( fmt::format( "{}{}{}", "ScionAssets", PATH_SEPARATOR, "soundfx.luac" ), soundFxPath ) )
		{
			SCION_ERROR( "Failed to add soundfx.luac to zip." );
			zip.close();
			return false;
		}
	}

	const std::string fontsPath{ fmt::format( "{}{}{}", m_Params.sTempFilepath, PATH_SEPARATOR, "fonts.luac" ) };
	if ( fs::exists( fontsPath ) )
	{
		if ( !zip.addFile( fmt::format( "{}{}{}", "ScionAssets", PATH_SEPARATOR, "fonts.luac" ), fontsPath ) )
		{
			SCION_ERROR( "Failed to add fonts.luac to zip." );
			zip.close();
			return false;
		}
	}

	zip.close();
	return true;
}

AssetPackager::AssetPackageStatus AssetPackager::SerializeAssetsByType( const rapidjson::Value& assets,
																		const std::filesystem::path& tempAssetsPath,
																		const std::string& sAssetTypeName,
																		const std::string& sContentPath,
																		SCION_UTIL::AssetType eAssetType )
{
	const std::string sAssetFile{ sAssetTypeName + ".s2dasset" };
	fs::path assetPath = tempAssetsPath / sAssetFile;

	std::unique_ptr<LuaSerializer> pLuaSerializer{ nullptr };
	try
	{
		pLuaSerializer = std::make_unique<LuaSerializer>( assetPath.string() );
	}
	catch ( const std::exception& ex )
	{
		std::string sError{ fmt::format( "Failed to serialize assets. {}", ex.what() ) };
		return { .sError = sError, .bSuccess = false };
	}

	if ( assets.HasMember( sAssetTypeName.c_str() ) )
	{
		const rapidjson::Value& assetArray = assets[ sAssetTypeName.c_str() ];
		if ( !assetArray.IsArray() )
		{
			std::string sError{ fmt::format( "Failed to serialize asset: File {} - Expecting \"{}\" must be an array",
											 tempAssetsPath.string(),
											 sAssetTypeName ) };

			return { .sError = sError, .bSuccess = false };
		}

		pLuaSerializer->StartNewTable( "S2D_Assets" );

		try
		{
			for ( const auto& jsonValue : assetArray.GetArray() )
			{
				std::string sPath{ sContentPath + PATH_SEPARATOR + jsonValue[ "path" ].GetString() };

				AssetConversionData conversionData{
					.sInAssetFile = sPath, .sAssetName = jsonValue[ "name" ].GetString(), .eType = eAssetType };

				if (eAssetType == SCION_UTIL::AssetType::FONT && jsonValue.HasMember("fontSize"))
				{
					conversionData.optFontSize = jsonValue[ "fontSize" ].GetFloat();
				}
				else if (eAssetType == SCION_UTIL::AssetType::TEXTURE && jsonValue.HasMember("bPixelArt"))
				{
					conversionData.optPixelArt = jsonValue[ "bPixelArt" ].GetBool();
				}

				ConvertAssetToLuaTable( *pLuaSerializer, conversionData);
			}
		}
		catch ( const std::exception& ex )
		{
			std::string sError{ fmt::format( "Failed to convert asset to lua table. {}", ex.what() ) };
			return { .sError = sError, .bSuccess = false };
		}

		pLuaSerializer->EndTable(); // S2D_Assets
	}

	return { .bSuccess = true };
}
} // namespace SCION_EDITOR
