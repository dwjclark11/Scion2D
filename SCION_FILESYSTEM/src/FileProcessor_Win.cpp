#include "ScionFilesystem/Process/FileProcessor.h"
#include "ScionUtilities/ScionUtilities.h"
#include "Logger/Logger.h"

#include <Windows.h>
#include <shellapi.h>
#include <fmt/format.h>

using namespace Scion::Utilities;

namespace Scion::Filesystem
{
bool FileProcessor::OpenApplicationFromFile( const std::string& sFilename, std::vector<const char*> params )
{
	std::string sParams = SeparateParams( params );

	INT_PTR shell = reinterpret_cast<INT_PTR>(
		::ShellExecuteW( NULL,
						 L"open",
						 StringUtils::ConvertAnsiToWide( sFilename ).c_str(),
						 !sParams.empty() ? StringUtils::ConvertAnsiToWide( sParams ).c_str() : L"",
						 L"",
						 SW_SHOWNORMAL ) );

	// If there is no default program set for the file type, prompt the user to choose an application.
	if ( shell == SE_ERR_NOASSOC || shell == SE_ERR_ASSOCINCOMPLETE )
	{
		shell = reinterpret_cast<INT_PTR>( ::ShellExecuteW(
			NULL,
			L"open",
			L"RUNDLL32.EXE",
			StringUtils::ConvertAnsiToWide( fmt::format( "shell32.dll, OpenAs_RunDLL {}", sFilename ) ).c_str(),
			L"",
			SW_SHOWNORMAL ) );
	}

	// If the shell code us greater that 32 == SUCCESS
	if ( shell >= 32 )
	{
		return true;
	}

	DWORD error{ GetLastError() };
	SCION_ERROR( "Failed to open file [{}]. Error: {}", sFilename, error );
	return true;
}

bool FileProcessor::OpenFileLocation( const std::string& sFilename )
{
	if ( !fs::exists( fs::path{ sFilename } ) )
	{
		SCION_ERROR( "Failed to open file location [{}]. File does not exist.", sFilename );
		return false;
	}

	std::wstring sCommand{ std::format( L"explorer /select, \"{}\"", StringUtils::ConvertUtf8ToWide( sFilename ) ) };

	STARTUPINFOW si{};
	PROCESS_INFORMATION pi{};
	si.cb = sizeof( si );

	if ( CreateProcessW( nullptr,
						 sCommand.data(),
						 nullptr,
						 nullptr,
						 FALSE,
						 CREATE_NO_WINDOW | DETACHED_PROCESS,
						 nullptr,
						 nullptr,
						 &si,
						 &pi ) )
	{
		CloseHandle( pi.hThread );
		CloseHandle( pi.hProcess );
		return true;
	}

	DWORD error{ GetLastError() };
	SCION_ERROR( "Failed to open file location [{}]. {}", sFilename, error );

	return false;
}

} // namespace Scion::Filesystem
