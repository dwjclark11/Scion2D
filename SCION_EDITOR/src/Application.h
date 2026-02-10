#pragma once
#include <SDL.h>

namespace Scion::Windowing
{
class Window;
}

namespace Scion::Editor::Events
{
struct CloseEditorEvent;
}

namespace Scion::Editor
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

	void OnCloseEditor( Scion::Editor::Events::CloseEditorEvent& close );

	Application();
	~Application() = default;
	Application( const Application& ) = delete;
	Application& operator=( const Application& ) = delete;

  private:
	std::unique_ptr<Scion::Windowing::Window> m_pWindow;
	std::unique_ptr<class Hub> m_pHub;

	SDL_Event m_Event;
	bool m_bIsRunning;
};
} // namespace Scion::Editor
