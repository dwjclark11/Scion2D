#pragma once
#include <memory>
#include <SDL.h>

namespace SCION_WINDOWING
{
class Window;
}

#ifdef SCION_OPENGL_DEBUG_CALLBACK
namespace SCION_RENDERING
{
class OpenGLDebugger;
}
#endif //  SCION_OPENGL_DEBUG_CALLBACK

namespace SCION_EDITOR
{
class Application
{
  private:
	std::unique_ptr<SCION_WINDOWING::Window> m_pWindow;

	SDL_Event m_Event;
	bool m_bIsRunning;

  private:
	bool Initialize();
	bool LoadShaders();
	bool LoadEditorTextures();

	void ProcessEvents();
	void Update();
	void Render();

	void CleanUp();

	bool CreateDisplays();
	bool InitImGui();
	void Begin();
	void End();
	void RenderImGui();

	Application();
	~Application() = default;
	Application( const Application& ) = delete;
	Application& operator=( const Application& ) = delete;

  public:
	static Application& GetInstance();

	void Run();

  private:
#ifdef SCION_OPENGL_DEBUG_CALLBACK
	std::unique_ptr<SCION_RENDERING::OpenGLDebugger> m_openGLDebugger;
#endif //  SCION_OPENGL_DEBUG_CALLBACK
};
} // namespace SCION_EDITOR
