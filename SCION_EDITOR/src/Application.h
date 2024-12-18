#pragma once
#include <memory>
#include <SDL.h>

namespace SCION_WINDOWING
{
class Window;
}

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

	/* TODO: Move to class to handle ImGui */
	bool InitImGui();
	void Begin();
	void End();
	void RenderImGui();

	void RegisterEditorMetaFunctions();

	Application();
	~Application() = default;
	Application( const Application& ) = delete;
	Application& operator=( const Application& ) = delete;

  public:
	static Application& GetInstance();

	void Run();
};
} // namespace SCION_EDITOR
