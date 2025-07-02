#include "Core/Scripting/LuaFilesystemBindings.h"
#include "Logger/Logger.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace SCION_CORE::Scripting
{
void LuaFilesystem::CreateLuaFileSystemBind( sol::state& lua )
{
	lua.new_usertype<LuaFilesystem>(
		"Filesystem",
		sol::no_constructor,
		"file_exists",
		[]( const std::string& sFilepath ) { return fs::exists( fs::path{ sFilepath } ); },
		"remove_file",
		[]( const std::string& sFilepath ) {
			std::error_code ec;
			if ( !fs::exists( fs::path{ sFilepath }, ec ) )
			{
				SCION_ERROR( "Failed to remove file [{}] - Error: {}", sFilepath, ec.message() );
				return false;
			}

			if ( !fs::remove( sFilepath, ec ) )
			{
				SCION_ERROR( "Failed to remove file [{}] - Error: {}", sFilepath, ec.message() );
				return false;
			}

			return true;
		},
		"create_directory",
		[]( const std::string& sDirectorPath ) {
			// Check to see if the directory already exists
			std::error_code ec;
			if ( fs::is_directory( sDirectorPath, ec ) || fs::exists( sDirectorPath, ec ) )
			{
				SCION_ERROR( "Directory [{}] already exists. - {}", sDirectorPath, ec.message() );
				return false;
			}

			if ( !fs::create_directory( sDirectorPath, ec ) )
			{
				SCION_ERROR( "Failed to create directory [{}]. - Error: {}", sDirectorPath, ec.message() );
				return false;
			}

			return true;
		},
		"create_directories",
		[]( const std::string& sDirectorPath ) {
			// Check to see if the directory already exists
			std::error_code ec;
			if ( fs::is_directory( sDirectorPath, ec ) || fs::exists( sDirectorPath, ec ) )
			{
				SCION_ERROR( "Directory [{}] already exists. - {}", sDirectorPath, ec.message() );
				return false;
			}

			if ( !fs::create_directories( sDirectorPath, ec ) )
			{
				SCION_ERROR( "Failed to create directory [{}]. - Error: {}", sDirectorPath, ec.message() );
				return false;
			}

			return true;
		},
		"write_file",
		[]( const std::string& sFilename, const std::string& data ) {
			// Create ofstream file
			std::ofstream outFile;
			outFile.open( sFilename );

			if ( !outFile.is_open() )
			{
				SCION_ERROR( "Could Not Open File: [{}]. Unable to write data to file!", sFilename );
				return false;
			}

			// Write the data to the file
			outFile << data;
			// Close the file
			outFile.close();
			// Success return true
			return true;
		},
		"read_file",
		[]( const std::string& sFilename ) {
			std::ifstream ifs( sFilename );
			std::string data{};
			// If the stream is good, read the steam into the data
			if ( ifs.good() )
			{
				data.assign( std::istreambuf_iterator<char>( ifs ), std::istreambuf_iterator<char>() );
			}
			else
			{
				// Set the data to an empty string
				data = "";
			}
			return data;
		} );
}
} // namespace SCION_CORE::Scripting
