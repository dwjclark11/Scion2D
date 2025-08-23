#pragma once
#include <SDL.h>

namespace SCION_WINDOWING
{
class Window;
}

namespace SCION_EDITOR::Events
{
struct CloseEditorEvent;
}

namespace SCION_EDITOR
{
class Application
{
  public:
	static Application& GetInstance();
	void Run();

  private:
	bool Initialize();
	bool InitApp();
	bool LoadShaders();
	bool LoadEditorTextures();

	void ProcessEvents();
	void Update();
	void UpdateInputs();
	void Render();

	void CleanUp();
	bool CreateDisplays();

	void InitDisplays();
	void RenderDisplays();

	void RegisterEditorMetaFunctions();

	void OnCloseEditor( SCION_EDITOR::Events::CloseEditorEvent& close );

	Application();
	~Application() = default;
	Application( const Application& ) = delete;
	Application& operator=( const Application& ) = delete;

  private:
	std::unique_ptr<SCION_WINDOWING::Window> m_pWindow;
	std::unique_ptr<class Hub> m_pHub;

	SDL_Event m_Event;
	bool m_bIsRunning;
};
} // namespace SCION_EDITOR
