#pragma once
#include <SDL.h>

namespace SCION_WINDOWING
{
class Window;
}

namespace SCION_EDITOR
{

enum class EHubState
{
	Default,
	NewProject,
	CreateNew,
	OpenProject,
	Close,
	NoState
};

class Hub
{
  public:
	Hub( SCION_WINDOWING::Window& window );
	~Hub();
	bool Run();

  private:
	bool Initialize();
	void DrawGui();

	void DrawDefault();
	void DrawNewProject();
	void DrawOpenProject();

	void ProcessEvents();
	void Update();
	void Render();

  private:
	SCION_WINDOWING::Window& m_Window;
	bool m_bRunning;
	bool m_bLoadError;
	SDL_Event m_Event;
	EHubState m_eState;

	float m_Width;
	float m_Height;

	std::string m_sNewProjectName;
	std::string m_sNewProjectPath;
	std::string m_sPrevProjectPath;
	std::string m_sPrevProjectName;
};
} // namespace SCION_EDITOR
