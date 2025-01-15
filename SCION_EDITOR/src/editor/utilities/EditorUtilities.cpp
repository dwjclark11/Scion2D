#include "EditorUtilities.h"
#include <filesystem>
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"

namespace fs = std::filesystem;

namespace SCION_EDITOR
{
EFileType GetFileType( const std::string& sPath )
{
	fs::path path{ sPath };

	if ( !fs::exists( path ) )
		return EFileType::INVALID_TYPE;

	if ( fs::is_directory( path ) )
		return EFileType::FOLDER;

	std::string sExt = path.extension().string();
	if ( sExt == ".wav" || sExt == ".mp3" || sExt == ".ogg" )
		return EFileType::SOUND;
	else if ( sExt == ".png" || sExt == ".mp3" || sExt == ".ogg" )
		return EFileType::IMAGE;
	else if ( sExt == ".lua" || sExt == ".cpp" || sExt == ".h" || sExt == ".txt" )
		return EFileType::TXT;
	else
		return EFileType::TXT;
}

std::vector<std::string> SplitStr( const std::string& str, char delimiter )
{
	std::vector<std::string> tokens;
	size_t start{ 0 };
	size_t end{ str.find( delimiter ) };

	while ( end != std::string::npos )
	{
		tokens.push_back( str.substr( start, end - start ) );
		start = end + 1;
		end = str.find( delimiter, start );
	}
	tokens.push_back( str.substr( start ) );
	return tokens;
}

SCION_RENDERING::Texture* GetIconTexture( const std::string& sPath )
{
	auto& assetManager = ASSET_MANAGER();
	switch ( GetFileType( sPath ) )
	{
	case EFileType::SOUND: return assetManager.GetTexture( "S2D_music_icon" ).get();
	case EFileType::IMAGE: return assetManager.GetTexture( "S2D_image_icon" ).get();
	case EFileType::TXT: return assetManager.GetTexture( "S2D_file_icon" ).get();
	case EFileType::FOLDER: return assetManager.GetTexture( "S2D_folder_icon" ).get();
	default: return nullptr;
	}
}
} // namespace SCION_EDITOR
