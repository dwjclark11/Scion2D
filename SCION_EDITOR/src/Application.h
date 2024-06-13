#pragma once
#include <glad/glad.h>
#include <Windowing/Window/Window.h>
#include <Core/ECS/Registry.h>

namespace SCION_EDITOR
{
class Application
{
  private:
	std::unique_ptr<SCION_WINDOWING::Window> m_pWindow;
	std::unique_ptr<SCION_CORE::ECS::Registry> m_pRegistry;

	SDL_Event m_Event;
	bool m_bIsRunning;

  private:
	bool Initialize();
	bool LoadShaders();

	void ProcessEvents();
	void Update();
	void Render();

	void CleanUp();

	// TEMP IMGUI TESTING --> TODO: REMOVE TO OWN CLASS
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
};
} // namespace SCION_EDITOR