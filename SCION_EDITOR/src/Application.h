#pragma once
#include <glad/glad.h>
#include <Windowing/Window/Window.h>
#include <Core/ECS/Registry.h>

namespace SCION_EDITOR {
	class Application
	{
	private:
		std::unique_ptr<SCION_WINDOWING::Window> m_pWindow;
		std::unique_ptr<SCION_CORE::ECS::Registry> m_pRegistry;

		SDL_Event m_Event;
		bool m_bIsRunning;
		
		// TEMP for testing
		GLuint VAO, VBO, IBO;
	private:
		bool Initialize();
		bool LoadShaders();

		void ProcessEvents();
		void Update();
		void Render();

		void CleanUp();

		Application();
	public:
		static Application& GetInstance();
		~Application();

		void Run();
	};
}