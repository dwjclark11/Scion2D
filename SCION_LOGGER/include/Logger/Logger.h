#pragma once
#include <string>
#include <string_view>
#include <source_location>
#include <vector>
#include <cassert>

/*
 * @brief Variadic Macro for logging Information. This macro takes in a string message, followed by the
 * necessary arguments.
 * @param Takes an std::string_view or string in the form of "This is a log value: {0}, and {1}", followed by
 * the arguments
 */
#define SCION_LOG( x, ... ) SCION_LOGGER::Logger::GetInstance().Log( x __VA_OPT__(, ) __VA_ARGS__ )

/*
 * @brief Variadic Macro for logging warnings. This macro takes in a string message, followed by the
 * necessary arguments.
 * @param Takes an std::string_view or string in the form of "This is a log value: {0}, and {1}", followed by
 * the arguments
 */
#define SCION_WARN( x, ... ) SCION_LOGGER::Logger::GetInstance().Warn( x __VA_OPT__(, ) __VA_ARGS__ )

/*
 * @brief Variadic Macro for logging Errors. This macro takes in a string message, followed by the
 * necessary arguments.
 * @param Takes an std::string_view or string in the form of "This is a log value: {0}, and {1}", followed by
 * the arguments
 */
#define SCION_ERROR( x, ... )                                                                                \
	SCION_LOGGER::Logger::GetInstance().Error( std::source_location::current(), x __VA_OPT__(, ) __VA_ARGS__ )

#define SCION_ASSERT( x ) assert( x )
#define SCION_INIT_LOGS( console, retain ) SCION_LOGGER::Logger::GetInstance().Init( console, retain )
#define SCION_LOG_ADDED() SCION_LOGGER::Logger::GetInstance().LogAdded()
#define SCION_RESET_ADDED() SCION_LOGGER::Logger::GetInstance().ResetLogAdded()
#define SCION_GET_LOGS() SCION_LOGGER::Logger::GetInstance().GetLogs()
#define SCION_CLEAR_LOGS() SCION_LOGGER::Logger::GetInstance().ClearLogs()

namespace SCION_LOGGER
{

struct LogEntry
{
	enum class LogType
	{
		INFO,
		WARN,
		ERR,
		NONE
	};
	LogType type{ LogType::INFO };
	std::string log{ "" };
};

class Logger
{
  private:
	std::vector<LogEntry> m_LogEntries;
	bool m_bLogAdded{ false }, m_bInitialized{ false }, m_bConsoleLog{ true }, m_bRetainLogs{ true };

	Logger() = default;

	struct LogTime
	{
		std::string day, dayNumber, month, year, time;
		LogTime( const std::string& date );
	};

	std::string CurrentDateTime();

  public:
	static Logger& GetInstance();

	~Logger() = default;
	// Make the logger non-copyable
	Logger( const Logger& ) = delete;
	Logger& operator=( const Logger& ) = delete;

	void Init( bool consoleLog = true, bool retainLogs = true );

	template <typename... Args>
	void Log( const std::string_view message, Args&&... args );

	template <typename... Args>
	void Warn( const std::string_view message, Args&&... args );

	template <typename... Args>
	void Error( std::source_location location, const std::string_view message, Args&&... args );

	void LuaLog( const std::string_view message );
	void LuaWarn( const std::string_view message );
	void LuaError( const std::string_view message );

	inline void ClearLogs() { m_LogEntries.clear(); }
	inline const std::vector<LogEntry>& GetLogs() { return m_LogEntries; }
	inline void ResetLogAdded() { m_bLogAdded = false; }
	inline bool LogAdded() const { return m_bLogAdded; }

};
} // namespace SCION_LOGGER

#include "Logger.inl"

