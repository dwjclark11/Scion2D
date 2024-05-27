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
#ifdef _WIN32
#define SCION_LOG(x, ...) SCION_LOGGER::Logger::GetInstance().Log(x, __VA_ARGS__)
#else
#define SCION_LOG(x, ...) SCION_LOGGER::Logger::GetInstance().Log(x, ##__VA_ARGS__)
#endif 

/*
* @brief Variadic Macro for logging warnings. This macro takes in a string message, followed by the
* necessary arguments.
* @param Takes an std::string_view or string in the form of "This is a log value: {0}, and {1}", followed by
* the arguments
*/
#ifdef _WIN32
#define SCION_WARN(x, ...) SCION_LOGGER::Logger::GetInstance().Warn(x, __VA_ARGS__)
#else
#define SCION_WARN(x, ...) SCION_LOGGER::Logger::GetInstance().Warn(x, ##__VA_ARGS__)
#endif

/*
* @brief Variadic Macro for logging Errors. This macro takes in a string message, followed by the
* necessary arguments.
* @param Takes an std::string_view or string in the form of "This is a log value: {0}, and {1}", followed by
* the arguments
*/
#ifdef _WIN32
#define SCION_ERROR(x, ...) SCION_LOGGER::Logger::GetInstance().Error(std::source_location::current(), x, __VA_ARGS__)
#else
#define SCION_ERROR(x, ...) SCION_LOGGER::Logger::GetInstance().Error(std::source_location::current(), x, ##__VA_ARGS__)
#endif

#define SCION_ASSERT(x) assert(x);
#define SCION_INIT_LOGS(console, retain) SCION_LOGGER::Logger::GetInstance().Init(console, retain);

namespace SCION_LOGGER {

	struct LogEntry
	{
		enum class LogType { INFO, WARN, ERR, NONE };
		LogType type{ LogType::INFO };
		std::string log{""};
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
			LogTime(const std::string& date);
		};

		std::string CurrentDateTime();

	public:
		static Logger& GetInstance();
		
		~Logger() = default;
		// Make the logger non-copyable
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;


		void Init(bool consoleLog = true, bool retainLogs = true);

		template <typename... Args>
		void Log(const std::string_view message, Args&&... args);

		template <typename... Args>
		void Warn(const std::string_view message, Args&&... args);

		template <typename... Args>
		void Error(std::source_location location, const std::string_view message, Args&&... args);

	};
}

#include "Logger.inl"