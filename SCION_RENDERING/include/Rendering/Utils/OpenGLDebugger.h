#pragma once

#include <vector>

namespace SCION_RENDERING
{
enum class OpenGLDebuggerSeverity : short
{
	Disable = 0,
	Notification,
	Low,
	Medium,
	High
};

class OpenGLDebugger final
{
  private:
	OpenGLDebugger() = delete;
	OpenGLDebugger( const OpenGLDebugger& ) = delete;
	OpenGLDebugger& operator=( const OpenGLDebugger& ) = delete;
	OpenGLDebugger( OpenGLDebugger&& ) = delete;
	OpenGLDebugger& operator=( OpenGLDebugger&& ) = delete;
	~OpenGLDebugger() = delete;

  public:
	/*
	 * @brief Initializes the OpenGL debug callback. A valid OpenGL context is necessary.
	 */
	static void init();

	/*
	 * @brief Initializes the OpenGL debug callback. A valid OpenGL context is necessary.
	 * @param The list of Ids you want to de-activate by default.
	 */
	static void init( const std::vector<unsigned int>& p_ignoreList );

	/*
	 * @brief Pushes an OpenGL error Id to the ignore list.
	 * @param The error Id given by the error callback you want to de-activate.
	 */
	static void push( unsigned int p_errorId );

	/*
	 * @brief Removes an OpenGL error Id from the ignore list.
	 * @param The error Id given by the error callback to re-activate.
	 */
	static void pop( unsigned int p_errorId );

	/*
	 * @brief Clears the ignore list.
	 */
	static void clear();

	/*
	 * @brief Retrieves the ignore list.
	 */
	static std::vector<unsigned int> getIgnoreList();

	static void breakOnError( bool );
	static void breakOnWarning( bool );
	static void setSeverityLevel( OpenGLDebuggerSeverity );
};
} // namespace SCION_RENDERING
