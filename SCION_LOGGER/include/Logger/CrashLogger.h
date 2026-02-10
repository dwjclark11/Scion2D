#pragma once
#include <fstream>
#include <functional>

struct lua_State;

#define SCION_CRASH_LOGGER() Scion::Logger::CrashLogger::GetInstance()
#define SCION_INIT_CRASH_LOGS() SCION_CRASH_LOGGER().Initialize()

namespace Scion::Logger
{
class CrashLogger
{
  public:
	static CrashLogger& GetInstance();

	CrashLogger( const CrashLogger& ) = delete;
	CrashLogger& operator=( const CrashLogger& ) = delete;

	void Initialize();

	/**
	 * @brief Sets the project path and ensures the crash logs directory exists.
	 *
	 * Verifies that the provided project path exists and sets it
	 * as the base directory for crash logs. If the `crash_logs` directory does
	 * not exist, it attempts to create it.
	 *
	 * - If the project path is invalid, an error is logged, and the path is cleared.
	 * - If the `crash_logs` directory cannot be created, an error is logged, and
	 *   the path is cleared.
	 * - On success, the project path is set to the crash logs directory.
	 *
	 * @param sProjectPath The base project path to set.
	 */
	void SetProjectPath( const std::string& sProjectPath );

	inline const std::string& GetProjectPath() const { return m_sProjectPath; }
	inline void SetLuaState( lua_State* pLuaState ) { m_pLuaState = pLuaState; }
	inline void SetUserDefinedCrashHandler( std::function<void( int )> func ) { m_UserCrashHandlerFunc = func; }

	/**
	 * @brief Handles program crashes by logging relevant debug information.
	 *
	 * Extracts the crash location, prints details to `stderr`, and writes them
	 * to a crash log file. Also logs the Lua stack trace before exiting.
	 *
	 * @param signal The crash signal received.
	 */
	static void CrashHandler( int signal );

  private:
	/**
	 * Private constructor to enforce Singleton pattern.
	 */
	CrashLogger() = default;

	/**
	 * @brief Logs the current Lua stack trace to the provided output stream.
	 *
	 * This function checks if a valid Lua state is available before retrieving
	 * the stack trace using `luaL_traceback()`. The traceback is then written
	 * to the output stream.
	 *
	 * @param out The output stream to write the Lua stack trace to.
	 */
	void LogLuaStackTrace( std::ostream& out );

	/**
	 * @brief Logs the current Lua stack trace to a file.
	 *
	 * This function checks if a valid Lua state is available before retrieving
	 * the stack trace using `luaL_traceback()`. The traceback is then written
	 * to the specified output file.
	 *
	 * @param outFile The output file stream to write the Lua stack trace to.
	 */
	void LogLuaStackTrace( std::ofstream& outFile );

	/*
	 * @brief Launches a separate executable that will display the current
	 * or last log entry to the user.
	 *
	 * @param sFilename The filename to the crash logs to be loaded.
	 */
	static void LaunchCrashReporter( const std::string& sFilename );

	static std::string GetCurrentTimestamp();

	/**
	 * @brief Extracts the crash location from the call stack.
	 *
	 * This function analyzes the stack trace to determine the source file and
	 * line number where the crash occurred. The extracted crash location is
	 * stored in `sCrashFile` and `CrashLine`.
	 */
	static void ExtractCrashLocation();

	/**
	 * @brief Prints the highlighted source line where the crash occurred.
	 *
	 * This function attempts to open the source file determined during extraction
	 * and prints the line corresponding to indicate where the
	 * crash occurred.
	 *
	 * - If the crash location is unknown, an error message is printed.
	 * - If the source file cannot be opened, an error message is printed.
	 * - If the file is successfully read, the crash line is highlighted.
	 *
	 * @param out The output stream to write the highlighted source line to.
	 */
	static void PrintHighlightedSourceLine( std::ostream& out );

	/**
	 * @brief Prints the highlighted source line where the crash occurred.
	 *
	 * This function attempts to open the source file determined during extraction
	 * and prints the line corresponding to indicate where the
	 * crash occurred.
	 *
	 * - If the crash location is unknown, an error message is printed.
	 * - If the source file cannot be opened, an error message is printed.
	 * - If the file is successfully read, the crash line is highlighted.
	 *
	 * @param out The output file to write the highlighted source line to.
	 */
	static void PrintHighlightedSourceLine( std::ofstream& outFile );

  private:
	/* User-defined crash handler function. */
	std::function<void( int )> m_UserCrashHandlerFunc{ nullptr };
	/* Pointer to the active lua state. */
	lua_State* m_pLuaState{ nullptr };
	/* Flag to determine if the logger was initialized. */
	bool m_bInitialized{ false };
	/* The specified project path. Used to keep logs with separate projects. */
	std::string m_sProjectPath{ };
	/* Stores the filename to where the crash occurred. */
	static inline std::string sCrashFile{ };
	/* Stores the line number where the crash occurred. */
	static inline int CrashLine{ -1 };
};
} // namespace Scion::Logger
