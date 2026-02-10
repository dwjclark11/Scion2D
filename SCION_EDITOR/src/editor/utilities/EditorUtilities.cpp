#include "editor/utilities/EditorUtilities.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/ProjectInfo.h"

// clang-format off
#ifdef _WIN32
static const std::unordered_set<std::string> g_setReservedSystemDirs
{
	"c:\\windows",
	"c:\\program files",
	"c:\\program files (x86)",
	"c:\\users\\default",
	"c:\\users\\public",
	"c:\\sysyem32"
};
#else
static const std::unordered_set<std::string> g_setReservedSystemDirs
{
	"/proc",
	"/sys",
	"/dev",
	"/run",
	"/boot",
	"/lib",
	"/lib64"
};
#endif
// clang-format on

namespace Scion::Editor
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

Scion::Rendering::Texture* GetIconTexture( const std::string& sPath )
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

bool IsReservedPathOrFile( const std::filesystem::path& path )
{
	std::string lowerPath = path.string();
	std::ranges::transform( lowerPath, lowerPath.begin(), ::tolower );
	return std::ranges::any_of( g_setReservedSystemDirs,
								[ & ]( const auto& dir ) { return lowerPath.starts_with( dir ); } );
}

bool IsDefaultProjectPathOrFile( const std::filesystem::path& path, const Scion::Core::ProjectInfo& projectInfo )
{
	if ( fs::is_directory(path) )
	{
		bool bIsProjectPath = std::ranges::any_of( projectInfo.GetProjectPaths(),
												   [ & ]( const auto& pair ) { return pair.second == path; } );

		if ( bIsProjectPath )
			return true;
	}

	if ( fs::is_regular_file(path) )
	{
		auto optMainLuaScript = projectInfo.GetMainLuaScriptPath();
		if ( optMainLuaScript && *optMainLuaScript == path )
			return true;

		auto optScriptList = projectInfo.GetScriptListPath();
		if ( optScriptList && *optScriptList == path )
			return true;
	}

	return false;
}

} // namespace Scion::Editor
