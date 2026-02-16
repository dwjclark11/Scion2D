#include "Logger/Logger.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
constexpr WORD GREEN = 2;
constexpr WORD RED = 4;
constexpr WORD YELLOW = 6;
constexpr WORD WHITE = 7;
#else
static const char* GREEN = "\033[0;32m";
static const char* YELLOW = "\033[0;33m";
static const char* RED = "\033[0;31m";
static const char* WHITE = "\033[0;30m";
static const char* CLOSE = "\022[0m";
#endif

#include <chrono>

namespace Scion::Logger
{

Logger::LogTime::LogTime( const std::string& date )
	: day{ date.substr( 0, 3 ) }
	, dayNumber{ date.substr( 8, 2 ) }
	, month{ date.substr( 4, 3 ) }
	, year{ date.substr( 20, 4 ) }
	, time{ date.substr( 11, 8 ) }
{
	// Ensure that the day number does not contain a space
	if (!dayNumber.empty() && std::isspace(dayNumber[0]))
	{
		dayNumber[ 0 ] = '0';
	}
}

std::string Logger::CurrentDateTime()
{
	auto time = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );

	char buf[ 30 ];
#ifdef _WIN32
	ctime_s( buf, sizeof( buf ), &time );
#else
	ctime_r( &time, buf );
#endif
	LogTime logTime{ std::string{ buf } };
	return fmt::format( "{0}-{1}-{2} {3}", logTime.year, logTime.month, logTime.dayNumber, logTime.time );
}

Logger& Logger::GetInstance()
{
	static Logger instance{};
	return instance;
}

void Logger::Init( bool consoleLog, bool retainLogs )
{
	assert( !m_bInitialized && "Don not call Initialize more than once!" );

	if ( m_bInitialized )
	{
		std::cout << "Logger has already been initialized!" << std::endl;
		return;
	}

	m_bConsoleLog = consoleLog;
	m_bRetainLogs = retainLogs;
	m_bInitialized = true;
}

void Logger::WriteConsoleLog( std::string_view sv, LogEntry::LogType eType )
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	switch ( eType )
	{
	case LogEntry::LogType::INFO: SetConsoleTextAttribute( hConsole, GREEN ); break;
	case LogEntry::LogType::WARN: SetConsoleTextAttribute( hConsole, YELLOW ); break;
	case LogEntry::LogType::ERR: SetConsoleTextAttribute( hConsole, RED ); break;
	case LogEntry::LogType::NONE: break;
	}
	std::cout << sv << "\n";
	SetConsoleTextAttribute( hConsole, WHITE );
#else
	switch ( eType )
	{
	case LogEntry::LogType::INFO: std::cout << GREEN << sv << CLOSE << "\n"; break;
	case LogEntry::LogType::WARN: std::cout << YELLOW << sv << CLOSE << "\n"; break;
	case LogEntry::LogType::ERR: std::cout << RED << sv << CLOSE << "\n"; break;
	case LogEntry::LogType::NONE: std::cout << WHITE << sv << CLOSE << "\n"; break;
	}
#endif
}

void Logger::LuaLog( const std::string_view message )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "LUA [INFO]: " << CurrentDateTime() << " - " << message << "\n";

	if ( m_bConsoleLog )
	{
#ifdef _WIN32
		HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
		SetConsoleTextAttribute( hConsole, GREEN );
		std::cout << ss.str();
		SetConsoleTextAttribute( hConsole, WHITE );
#else
		std::cout << GREEN << ss.str() << CLOSE << "\n\n";
#endif
	}

	if ( m_bRetainLogs )
	{
		m_LogEntries.emplace_back( LogEntry::LogType::INFO, ss.str() );
	}
}

void Logger::LuaWarn( const std::string_view message )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "LUA [WARN]: " << CurrentDateTime() << " - " << message << "\n";

	if ( m_bConsoleLog )
	{
#ifdef _WIN32
		HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
		SetConsoleTextAttribute( hConsole, YELLOW );
		std::cout << ss.str();
		SetConsoleTextAttribute( hConsole, WHITE );
#else
		std::cout << YELLOW << ss.str() << CLOSE << "\n\n";
#endif
	}

	if ( m_bRetainLogs )
	{
		m_LogEntries.emplace_back( LogEntry::LogType::WARN, ss.str() );
	}
}

void Logger::LuaError( const std::string_view message )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "LUA [ERROR]: " << CurrentDateTime() << " - " << message << "\n";

	if ( m_bConsoleLog )
	{
#ifdef _WIN32
		HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
		SetConsoleTextAttribute( hConsole, RED );
		std::cout << ss.str();
		SetConsoleTextAttribute( hConsole, WHITE );
#else
		std::cout << RED << ss.str() << CLOSE << "\n\n";
#endif
	}

	if ( m_bRetainLogs )
	{
		m_LogEntries.emplace_back( LogEntry::LogType::ERR, ss.str() );
	}
}
} // namespace Scion::Logger
