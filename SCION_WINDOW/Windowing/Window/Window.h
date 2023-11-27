#pragma once
#include <ScionUtilities/SDL_Wrappers.h>
#include <string>

namespace SCION_WINDOWING {
	class Window
	{
	private:
		WindowPtr m_pWindow;
		SDL_GLContext m_GLContext;
		std::string m_sTitle;
		int m_Width, m_Height, m_XPos, m_YPos;
		Uint32 m_WindowFlags;

	private:
		void CreateNewWindow(Uint32 flags);

	public:
		Window() : Window("default_window", 640, 480, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, true, NULL) { }
		Window(const std::string title, int width, int height, int x_pos, int y_pos, bool v_sync = true,
			Uint32 flags = (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_CAPTURE));
		~Window();

		void SetPosition(int x, int y);

		inline void SetGLContext(SDL_GLContext gl_context) { m_GLContext = gl_context; }
		inline SDL_GLContext& GetGLContext() { return m_GLContext; }
		inline WindowPtr& GetWindow() { return m_pWindow; }
		inline const std::string& GetWindowName() const { return m_sTitle; }
		
		inline const int GetXPos() const { return m_XPos; }
		inline const int GetYPos() const { return m_YPos; }

		inline void SetWidth(int width) { m_Width = width; }
		inline void SetHeight(int height) { m_Height = height; }
		inline const int GetWidth() const { return m_Width; }
		inline const int GetHeight() const { return m_Height; }

		void SetWindowName(const std::string& name);
	};
}