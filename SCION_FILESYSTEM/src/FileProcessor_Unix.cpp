#include "ScionFilesystem/Process/FileProcessor.h"
#include "ScionUtilities/ScionUtilities.h"

#include "Logger/Logger.h"

#include <unistd.h>
#include <errno.h>

using namespace SCION_UTIL;

namespace SCION_FILESYSTEM
{
bool FileProcessor::OpenApplicationFromFile( const std::string& sFilename, std::vector<const char*> params )
{
	// Separate any passed in parameters by a space and create a string
	std::string sParams = SeparateParams( params );

	// Create a new child process
	pid_t pid = fork();

	bool bSuccess{ true };

	// Check if the process id == 0, then try to open the file with it's default application
	if ( pid == 0 )
	{
		int error{ 0 };
		if ( !sParams.empty() )
		{
			error = execl( "/usr/bin/xdg-open", "xdg-open", sFilename.c_str(), sParams.c_str(), (char*)0 );
		}
		else
		{
			error = execl( "/usr/bin/xdg-open", "xdg-open", sFilename.c_str(), (char*)0 );
		}

		// Check for success
		exit( error );
	}

	// Log the error
	if ( pid == -1 )
	{
		// TODO: Get the correct reason/error
		SCION_ERROR( "Failed to open file [{}] in default application. Error: {}", sFilename, strerror( errno ) );
		bSuccess = false;
	}

	return bSuccess;
}

bool FileProcessor::OpenFileLocation(const std::string& sFilename)
{
	SCION_ERROR( "Not Implemented" );
	return false;
}

} // namespace SCION_FILESYSTEM
