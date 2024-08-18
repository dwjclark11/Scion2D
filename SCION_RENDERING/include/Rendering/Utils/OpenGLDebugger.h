#pragma once

namespace SCION_RENDERING
{
class OpenGLDebugger final
{
  public:
	OpenGLDebugger();
	OpenGLDebugger( const OpenGLDebugger& ) = delete;
	OpenGLDebugger& operator=( const OpenGLDebugger& ) = delete;
	OpenGLDebugger( OpenGLDebugger&& ) = delete;
	OpenGLDebugger& operator=( OpenGLDebugger&& ) = delete;
	~OpenGLDebugger();

  public:
	/*
	 * @brief Initializes the OpenGL debug callback. A valid OpenGL context is necessary.
	 */
	void init();
};
} // namespace SCION_RENDERING
