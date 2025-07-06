#include "ScionFilesystem/Utilities/FilesystemUtilities.h"
#include <cstdio>
#include <array>
#include <memory>
#include <stdexcept>

namespace SCION_FILESYSTEM
{
std::string SCION_FILESYSTEM::ExecCmdWithErrorOutput( const std::string& sCmd )
{
	std::array<char, 128> buffer;
	std::string sResult{};

#ifdef _WIN32
	// Redirect stderr (2) to stdout (1) with "2>&1" so we capture both.
	std::unique_ptr<FILE, decltype( &_pclose )> pipe( _popen( std::string{ sCmd + " 2>&1" }.c_str(), "r" ), _pclose );
#else
	std::unique_ptr<FILE, decltype( &pclose )> pipe( popen( std::string{ sCmd + " 2>&1" }.c_str(), "r" ), pclose );
#endif

	if ( !pipe )
	{
		throw std::runtime_error( "popen() failed" );
	}

	while ( fgets( buffer.data(), buffer.size(), pipe.get() ) != nullptr )
	{
		sResult += buffer.data();
	}

	return sResult;
}
} // namespace SCION_FILESYSTEM
