#include "Application.h"

#include <SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Rendering/Essentials/ShaderLoader.h>
#include <Rendering/Essentials/TextureLoader.h>
#include <Rendering/Essentials/Vertex.h>
#include <Rendering/Core/Camera2D.h>
#include <Logger/Logger.h>
#include <Core/ECS/Entity.h>
#include <Core/ECS/Components/SpriteComponent.h>
#include <Core/ECS/Components/Identification.h>
#include <Core/ECS/Components/TransformComponent.h>
#include <Core/Resources/AssetManager.h>

#include <Core/Systems/ScriptingSystem.h>
#include <Core/Systems/RenderSystem.h>

namespace SCION_EDITOR {

    bool Application::Initialize()
    {
		SCION_INIT_LOGS(true, true);

		// Init SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			std::string error = SDL_GetError();
			SCION_ERROR("Failed to initialize SDL: {0}", error);
			return false;
		}

		// Set up OpenGL
		if (SDL_GL_LoadLibrary(NULL) != 0)
		{
			std::string error = SDL_GetError();
			SCION_ERROR("Failed to Open GL Library: {0}", error);
			return false;
		}

		// Set the OpenGL attributes
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		// Set the number of bits per channel
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		// Create the Window
		m_pWindow = std::make_unique<SCION_WINDOWING::Window>(
			"Test Window", 
			640, 480, 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			true, SDL_WINDOW_OPENGL);

		if (!m_pWindow->GetWindow())
		{
			SCION_ERROR("Failed to create the window!");
			return false;
		}

		// Create the openGL context
		m_pWindow->SetGLContext(SDL_GL_CreateContext(m_pWindow->GetWindow().get()));

		if (!m_pWindow->GetGLContext())
		{
			std::string error = SDL_GetError();
			SCION_ERROR("Failed to create OpenGL context: {0}", error);
			return false;
		}

		SDL_GL_MakeCurrent(m_pWindow->GetWindow().get(), m_pWindow->GetGLContext());
		SDL_GL_SetSwapInterval(1);

		// Initialize Glad
		if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0)
		{
			SCION_ERROR("Failed to LoadGL --> GLAD");
			return false;
		}

		// Enable Alpha Blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto assetManager = std::make_shared<SCION_RESOURCES::AssetManager>();
		if (!assetManager)
		{
			SCION_ERROR("Failed to create the asset manager!");
			return false;
		}

		if (!assetManager->AddTexture("castle", "./assets/textures/castle.png", true))
		{
			SCION_ERROR("Failed to create and add the texture");
			return false;
		}

		// Add temp texture
		auto texture = assetManager->GetTexture("castle");

		SCION_LOG("Loaded Texture: [width = {0}, height = {1}]", texture.GetWidth(), texture.GetHeight());
		SCION_WARN("Loaded Texture: [width = {0}, height = {1}]", texture.GetWidth(), texture.GetHeight());

		m_pRegistry = std::make_unique<SCION_CORE::ECS::Registry>();

		SCION_CORE::ECS::Entity entity1{*m_pRegistry, "Ent1", "Test"};

		auto& transform = entity1.AddComponent<SCION_CORE::ECS::TransformComponent>(SCION_CORE::ECS::TransformComponent{
			.position = glm::vec2{ 10.f, 10.f },
				.scale = glm::vec2{ 3.f, 3.f },
				.rotation = 0.f
		}
		);

		auto& sprite = entity1.AddComponent<SCION_CORE::ECS::SpriteComponent>(SCION_CORE::ECS::SpriteComponent{
			.width = 16.f,
				.height = 16.f,
				.color = SCION_RENDERING::Color{ .r = 255, .g = 0, .b = 255, .a = 255 },
				.start_x = 0,
				.start_y = 28,
				.layer = 0,
				.texture_name = "castle"
		}
		);

		sprite.generate_uvs(texture.GetWidth(), texture.GetHeight());

		auto& id = entity1.GetComponent<SCION_CORE::ECS::Identification>();
		SCION_LOG("Name: {0}, GROUP: {1}, ID: {2}", id.name, id.group, id.entity_id);



		// Create the lua state
		auto lua = std::make_shared<sol::state>();

		if (!lua)
		{
			SCION_ERROR("Failed to create the lua state!");
			return false;
		}

		lua->open_libraries(sol::lib::base, sol::lib::math, sol::lib::os, sol::lib::table, sol::lib::io, sol::lib::string);

		if (!m_pRegistry->AddToContext<std::shared_ptr<sol::state>>(lua))
		{
			SCION_ERROR("Failed to add the sol::state to the registry context!");
			return false;
		}
		
		auto scriptSystem = std::make_shared<SCION_CORE::Systems::ScriptingSystem>(*m_pRegistry);
		if (!scriptSystem)
		{
			SCION_ERROR("Failed to create the script system!");
			return false;
		}
		
		if (!scriptSystem->LoadMainScript(*lua))
		{
			SCION_ERROR("Failed to load the main lua script!");
			return false;
		}
		
		if (!m_pRegistry->AddToContext<std::shared_ptr<SCION_CORE::Systems::ScriptingSystem>>(scriptSystem))
		{
			SCION_ERROR("Failed to add the script system to the registry context!");
			return false;
		}

		auto renderSystem = std::make_shared<SCION_CORE::Systems::RenderSystem>(*m_pRegistry);
		if (!renderSystem)
		{
			SCION_ERROR("Failed to create the render system!");
			return false;
		}

		if (!m_pRegistry->AddToContext<std::shared_ptr<SCION_CORE::Systems::RenderSystem>>(renderSystem))
		{
			SCION_ERROR("Failed to add the render system to the registry context!");
			return false;
		}


		// Create a temp camera
		auto camera = std::make_shared<SCION_RENDERING::Camera2D>();
		
		if (!m_pRegistry->AddToContext<std::shared_ptr<SCION_RESOURCES::AssetManager>>(assetManager))
		{
			SCION_ERROR("Failed to add the asset manager to the registry context!");
			return false;
		}

		if (!m_pRegistry->AddToContext<std::shared_ptr<SCION_RENDERING::Camera2D>>(camera))
		{
			SCION_ERROR("Failed to add the camera to the registry context!");
			return false;
		}

		if (!LoadShaders())
		{
			SCION_ERROR("Failed to load the shaders!");
			return false;
		}

		return true;
    }

    bool Application::LoadShaders()
    {
		auto& assetManager = m_pRegistry->GetContext<std::shared_ptr<SCION_RESOURCES::AssetManager>>();
		
		if (!assetManager)
		{
			SCION_ERROR("Failed to get the asset manager from the registry context");
			return false;
		}

		if (!assetManager->AddShader("basic", "assets/shaders/basicShader.vert", "assets/shaders/basicShader.frag"))
		{
			SCION_ERROR("Failed to add the shader to the asset manager");
			return false;
		}

		return true;
    }

    void Application::ProcessEvents()
    {
		// Process Events
		while (SDL_PollEvent(&m_Event))
		{
			switch (m_Event.type)
			{
			case SDL_QUIT:
				m_bIsRunning = false;
				break;
			case SDL_KEYDOWN:
				if (m_Event.key.keysym.sym == SDLK_ESCAPE)
					m_bIsRunning = false;
				break;
			default:
				break;
			}
		}
    }

    void Application::Update()
    {
		auto& camera = m_pRegistry->GetContext<std::shared_ptr<SCION_RENDERING::Camera2D>>();
		if (!camera)
		{
			SCION_ERROR("Failed to get the camera from the registry context!");
			return;
		}
		
		camera->Update();

		auto& scriptSystem = m_pRegistry->GetContext<std::shared_ptr<SCION_CORE::Systems::ScriptingSystem>>();
		scriptSystem->Update();

		auto view = m_pRegistry->GetRegistry().view<SCION_CORE::ECS::TransformComponent, SCION_CORE::ECS::SpriteComponent>();

		static float rotation{ 0.f };
		static float x_pos{ 10.f };
		static bool bMoveRight{ true };

		if (rotation >= 360.f)
			rotation = 0.f;

		if (bMoveRight && x_pos < 300.f)
			x_pos += 3;
		else if (bMoveRight && x_pos >= 300.f)
			bMoveRight = false;

		if (!bMoveRight && x_pos > 10.f)
			x_pos -= 3;
		else if (!bMoveRight && x_pos <= 10.f)
			bMoveRight = true;

		for (const auto& entity : view)
		{
			SCION_CORE::ECS::Entity ent{*m_pRegistry, entity};
			auto& transform = ent.GetComponent<SCION_CORE::ECS::TransformComponent>();

			transform.rotation = rotation;
			transform.position.x = x_pos;
		}
		
		rotation += bMoveRight ? 9 : -9;
    }

    void Application::Render()
    {
		auto& renderSystem = m_pRegistry->GetContext<std::shared_ptr<SCION_CORE::Systems::RenderSystem>>();
		
		glViewport(
			0, 0,
			m_pWindow->GetWidth(),
			m_pWindow->GetHeight()
		);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		auto& scriptSystem = m_pRegistry->GetContext<std::shared_ptr<SCION_CORE::Systems::ScriptingSystem>>();
		scriptSystem->Render();
		renderSystem->Update();

		SDL_GL_SwapWindow(m_pWindow->GetWindow().get());
    }

    void Application::CleanUp()
    {
		SDL_Quit();
    }

    Application::Application()
        : m_pWindow{nullptr}, m_pRegistry{nullptr}, m_Event{}, m_bIsRunning{true}
    {

    }

    Application& Application::GetInstance()
    {
		static Application app{};
		return app;
    }

    void Application::Run()
    {
		if (!Initialize())
		{
			SCION_ERROR("Initialization Failed!");
			return;
		}

		while (m_bIsRunning)
		{
			ProcessEvents();
			Update();
			Render();
		}

		CleanUp();
    }
}
