#pragma once
#include <string>

namespace SCION_FILESYSTEM
{
/**
 * @brief Executes a system command and captures both stdout and stderr output.
 *
 * This function runs the provided shell command and returns its combined output
 * (standard output and error output) as a std::string.
 *
 * @param sCmd The shell command to execute.
 * @return A string containing the full output (stdout + stderr) of the command.
 * @throws std::runtime_error if the command execution fails.
 */
std::string ExecCmdWithErrorOutput( const std::string& sCmd );

/**
* @brief Takes an input path string that may contain mixed direcory separators
* and returns a new string using the preffered directory separator for the
* current operating system.
*
* @param sPath A path string that my contain mixed or non-preferred separators.
* @return std::string The normalized path string with OS-preferred separators.
*/
std::string NormalizePath( const std::string& sPath );

} // namespace SCION_FILESYSTEM
