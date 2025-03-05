#include "Logger/CrashLogger.h"
#include "Logger/Logger.h"

#include <iostream>
#include <csignal> // For handling crash signals (SIGSEGV, SIGARBT)
#include <chrono>
#include <iomanip>
#include <sstream>
#include <sol/sol.hpp>
#include <source_location>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h> // This is needed for capturing stack traces
#pragma comment( lib, "dbghelp.lib" )
#define PATH_SEPARATOR "\\"
#elif __linux__
#include <execinfo.h> // For capturing stack traces in unix-based systems
#include <cxxabi.h>	  // Needed for demangling c++ symbols
#include <cstdlib>
#define PATH_SEPARATOR "/"
#endif

namespace fs = std::filesystem;

namespace SCION_LOGGER
{
CrashLogger& CrashLogger::GetInstance()
{
	static CrashLogger instance;
	return instance;
}

void CrashLogger::Initialize()
{
	if ( m_bInitialized )
	{
		SCION_ERROR( "Failed to initialize CrashLogger - Has already been initialized." );
		return;
	}
	// Segmentation fault (nullptr, invalid memory)
	std::signal( SIGSEGV, CrashHandler );
	// Abnormal termination (abort)
	std::signal( SIGABRT, CrashHandler );

	m_bInitialized = true;

	SCION_LOG( "Scion Crash Logger has been initialized." );
}

void CrashLogger::SetProjectPath( const std::string& sProjectPath )
{
	std::error_code ec;
	if ( !fs::exists( fs::path{ sProjectPath }, ec ) )
	{
		SCION_ERROR( "Failed to set project path. {}", ec.message() );
		m_sProjectPath.clear();
		return;
	}

	std::string sLogPath{ sProjectPath + PATH_SEPARATOR + "crash_logs" };

	if ( !fs::exists( fs::path{ sLogPath } ) )
	{
		// Create the logs folder if it does not exist
		std::error_code ec;
		if ( !fs::create_directory( sLogPath, ec ) )
		{
			SCION_ERROR( "Failed to create crash logs directory.", ec.message() );
			m_sProjectPath.clear();
			return;
		}
	}

	m_sProjectPath = sLogPath;
}

void CrashLogger::LogLuaStackTrace( std::ostream& out )
{
	if ( !m_pLuaState )
	{
		out << "[No active Lua state available]\n";
		return;
	}

	out << "\nLua Stack Trace\n";
	out << "------------------------------------------------------------\n";
	luaL_traceback( m_pLuaState, m_pLuaState, nullptr, 1 );
	out << lua_tostring( m_pLuaState, -1 ) << "\n";
	out << "------------------------------------------------------------\n";
	lua_pop( m_pLuaState, 1 );
}

void CrashLogger::LogLuaStackTrace( std::ofstream& outFile )
{
	if ( !m_pLuaState )
	{
		outFile << "[No active Lua state available]\n";
		return;
	}

	outFile << "\nLua Stack Trace\n";
	outFile << "------------------------------------------------------------\n";
	luaL_traceback( m_pLuaState, m_pLuaState, nullptr, 1 );
	outFile << lua_tostring( m_pLuaState, -1 ) << "\n";
	outFile << "------------------------------------------------------------\n";
	lua_pop( m_pLuaState, 1 );
}

std::string CrashLogger::GetCurrentTimestamp()
{
	auto now = std::chrono::system_clock::now();
	auto time_t_now = std::chrono::system_clock::to_time_t( now );
	std::tm tm_now{};

#ifdef _WIN32
	localtime_s( &tm_now, &time_t_now );
#else
	localtime_r( &time_t_now, &tm_now );
#endif // _WIN32

	std::ostringstream oss;
	oss << std::put_time( &tm_now, "%Y-%m-%d %H:%M:%S" );
	return oss.str();
}

void CrashLogger::ExtractCrashLocation()
{
#ifdef _WIN32
	SymSetOptions( SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS );
	// Retrive a handle to the current process
	HANDLE process = GetCurrentProcess();
	// Initialize the symbol handler for stack trace resolution
	SymInitialize( process, NULL, TRUE );

	void* stack[ 62 ];
	// Capture the call stack
	USHORT frames = CaptureStackBackTrace( 0, 62, stack, nullptr );

	// Skip the first few frames so we ignore the crash handler frames
	for ( USHORT i = 3; i < frames; ++i )
	{
		DWORD64 address = (DWORD64)( stack[ i ] );
		if ( SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc( sizeof( SYMBOL_INFO ) + 256, 1 ) )
		{
			symbol->SizeOfStruct = sizeof( SYMBOL_INFO );
			symbol->MaxNameLen = 255;

			DWORD displacement;
			IMAGEHLP_LINE64 line;
			line.SizeOfStruct = sizeof( IMAGEHLP_LINE64 );

			// Retrive the symbol information for the address and the file/line number
			if ( SymFromAddr( process, address, 0, symbol ) &&
				 SymGetLineFromAddr64( process, address, &displacement, &line ) )
			{
				std::string filename = line.FileName;
				if ( !filename.empty() && filename.find( "vcstartup" ) == std::string::npos )
				{
					if ( filename.find( ".cpp" ) != std::string::npos )
					{
						sCrashFile = filename;
						CrashLine = line.LineNumber;
						free( symbol );
						break;
					}
				}
			}

			free( symbol );
		}
	}
#elif __linux__
	/*
	 * Linux Version is not working they same way as on windows.
	 * I believe this has to do with the symbols being embedded in the target
	 * when compiling with gcc rather that into a separate pbd/debug file.
	 * I will have to investigate this more on my linux machine at a later date.
	 */
	void* buffer[ 50 ];
	// Capture the stack trace
	int nptrs = backtrace( buffer, 50 );
	// Get the symbol names for addresses
	char** symbols = backtrace_symbols( buffer, nptrs );

	if ( symbols )
	{
		// Skip the first few frames so we ignore the crash handler frames
		for ( int i = 3; i < nptrs; ++i )
		{
			std::string symbolStr( symbols[ i ] );

			if ( symbolStr.find( ".cpp" ) != std::string::npos )
			{
				std::ostringstream addr2lineCmd;
				addr2lineCmd << "addr2line -e " << symbols[ i ] << " " << buffer[ i ];

				// Execute addr2line to get file and line
				FILE* pipe = popen( addr2lineCmd.str().c_str(), "r" );
				char fileInfo[ 256 ] = { 0 };
				if ( pipe )
				{
					fgets( fileInfo, sizeof( fileInfo ), pipe );
					pclose( pipe );
					sCrashFile = fileInfo;
					CrashLine = atoi( strrchr( fileInfo, ':' ) + 1 );
					break;
				}
			}
		}
		free( symbols );
	}
#endif // _WIN32
}

void CrashLogger::PrintHighlightedSourceLine( std::ostream& out )
{
	if ( sCrashFile.empty() || CrashLine < 1 )
	{
		out << "[Error: Unable to determine crash location]\n";
		return;
	}

	std::ifstream sourceFile( sCrashFile );
	if ( !sourceFile )
	{
		out << "[Error: Unable to open source file: " << sCrashFile << "]\n";
		return;
	}

	std::string lineContent;
	int currentLine = 0;
	while ( std::getline( sourceFile, lineContent ) )
	{
		currentLine++;
		if ( currentLine == CrashLine )
		{
			out << ">>> " << lineContent << "  <-- Crash occurred here\n";
			break;
		}
	}
}

void CrashLogger::PrintHighlightedSourceLine( std::ofstream& outFile )
{
	if ( sCrashFile.empty() || CrashLine < 1 )
	{
		outFile << "[Error: Unable to determine crash location]\n";
		return;
	}

	std::ifstream sourceFile( sCrashFile );
	if ( !sourceFile )
	{
		outFile << "[Error: Unable to open source file: " << sCrashFile << "]\n";
		return;
	}

	std::string lineContent;
	int currentLine = 0;
	while ( std::getline( sourceFile, lineContent ) )
	{
		currentLine++;
		if ( currentLine == CrashLine )
		{
			outFile << ">>> " << lineContent << "  <-- Crash occurred here\n";
			break;
		}
	}
}

void CrashLogger::CrashHandler( int signal )
{
	ExtractCrashLocation();
	std::string sTimestamp{ GetCurrentTimestamp() };

	std::ostringstream ss;

	ss << "\n============================================================\n";
	ss << "[CRITICAL] Program crashed! Signal: " << signal << " (" << sTimestamp << ")" << "\n";
	ss << "============================================================\n";
	ss << "\nCrash Detected:\n------------------------------------------------------------\n";
	ss << "File: " << sCrashFile << "\n";
	ss << "Line: " << CrashLine << "\n";

	std::cerr << ss.str();

	PrintHighlightedSourceLine( std::cerr );

	auto& crashLogger = GetInstance();
	std::unique_ptr<std::ofstream> pLogFile{ nullptr };
	const std::string sProjectPath{ crashLogger.GetProjectPath() };
	if ( !sProjectPath.empty() )
	{
		// We want to open the file as an append, so we add new logs
		pLogFile = std::make_unique<std::ofstream>( sProjectPath + PATH_SEPARATOR + "crash_log.txt", std::ios::app );
	}
	else // Create the log in the program folder.
	{
		pLogFile = std::make_unique<std::ofstream>( "crash_log.txt", std::ios::app );
	}

	SCION_ASSERT( pLogFile && "Log File was not successfully created and opened." );

	// There is no file to log to, finish log write to console and exit
	if ( !pLogFile )
	{
		SCION_ERROR( "Failed to write to crash log file" );
		crashLogger.LogLuaStackTrace( std::cerr );
		std::exit( signal );
		return;
	}

	*pLogFile << ss.str();
	PrintHighlightedSourceLine( *pLogFile );

	crashLogger.LogLuaStackTrace( std::cerr );
	crashLogger.LogLuaStackTrace( *pLogFile );

	std::exit( signal );
}

} // namespace SCION_LOGGER
