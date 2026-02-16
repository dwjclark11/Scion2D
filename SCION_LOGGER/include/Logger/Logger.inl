#pragma once
#include "Logger.h"
#include <fmt/format.h>
#include <iostream>
#include <sstream>

namespace Scion::Logger
{
template <typename... Args>
void Logger::Log( const std::string_view message, Args&&... args )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "SCION [INFO]: " << CurrentDateTime() << " - " << fmt::vformat( message, fmt::make_format_args( args... ) );

	if ( m_bConsoleLog )
	{
		WriteConsoleLog( ss.str(), LogEntry::LogType::INFO );
	}

	if ( m_bRetainLogs )
	{
		m_LogEntries.emplace_back( LogEntry::LogType::INFO, ss.str() );
	}
}

template <typename... Args>
void Logger::Warn( const std::string_view message, Args&&... args )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "SCION [WARN]: " << CurrentDateTime() << " - " << fmt::vformat( message, fmt::make_format_args( args... ) );

	if ( m_bConsoleLog )
	{
		WriteConsoleLog( ss.str(), LogEntry::LogType::WARN );
	}

	if ( m_bRetainLogs )
	{
		m_LogEntries.emplace_back( LogEntry::LogType::WARN, ss.str() );
	}
}

template <typename... Args>
void Logger::Error( std::source_location location, const std::string_view message, Args&&... args )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "SCION [ERROR]: " << CurrentDateTime() << " - " << fmt::vformat( message, fmt::make_format_args( args... ) )
	   << "\nFUNC: " << location.function_name() << "\nLINE: " << location.line();

	if ( m_bConsoleLog )
	{
		WriteConsoleLog( ss.str(), LogEntry::LogType::ERR );
	}

	if ( m_bRetainLogs )
	{
		m_LogEntries.emplace_back( LogEntry::LogType::ERR, ss.str() );
	}
}

template <typename... Args>
void Logger::Error( const std::string_view message, Args&&... args )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "SCION [ERROR]: " << CurrentDateTime() << " - " << fmt::vformat( message, fmt::make_format_args( args... ) );

	if ( m_bConsoleLog )
	{
		WriteConsoleLog( ss.str(), LogEntry::LogType::ERR );
	}

	if ( m_bRetainLogs )
	{
		m_LogEntries.emplace_back( LogEntry::LogType::ERR, ss.str() );
	}
}
} // namespace Scion::Logger
