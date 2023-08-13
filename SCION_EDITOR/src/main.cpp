#define SDL_MAIN_HANDLED 1
#define NOMINMAX
#include <Windowing/Window/Window.h>
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
#include <entt.hpp>


/*
* This is just a temporary struct. We will be replacing this with the SpriteComponent later on
*/
struct UVs
{
	float u{ 0.f }, v{ 0.f }, uv_width{ 0.f }, uv_height{ 0.f };
};

struct TransformComponent
{
	glm::vec2 position{glm::vec2{0.f}}, scale{ glm::vec2{1.} };
	float rotation{ 0.f };
};

struct SpriteComponent
{
	float width{ 0.f }, height{ 0.f };
	UVs uvs{ .u = 0.f, .v = 0.f, .uv_width = 0.f, .uv_height = 0.f };

	SCION_RENDERING::Color color{.r = 255, .g = 255, .b = 255, .a = 255};
	int start_x{ 0 }, start_y{0};

	void generate_uvs(int textureWidth, int textureHeight)
	{
		uvs.uv_width = width / textureWidth;
		uvs.uv_height = height / textureHeight;

		uvs.u = start_x * uvs.uv_width;
		uvs.v = start_y * uvs.uv_height;
	}
};

int main()
{
	SCION_INIT_LOGS(true, true);

	bool running{ true };

	// Init SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::string error = SDL_GetError();
		SCION_ERROR("Failed to initialize SDL: {0}", error);
		running = false;
		return -1;
	}

	// Set up OpenGL
	if (SDL_GL_LoadLibrary(NULL) != 0)
	{
		std::string error = SDL_GetError();
		SCION_ERROR("Failed to Open GL Library: {0}", error);
		running = false;
		return -1;
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
	SCION_WINDOWING::Window window("Test Window", 640, 480, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, true, SDL_WINDOW_OPENGL);

	if (!window.GetWindow())
	{
		SCION_ERROR("Failed to create the window!");
		return -1;
	}

	// Create the openGL context
	window.SetGLContext(SDL_GL_CreateContext(window.GetWindow().get()));

	if (!window.GetGLContext())
	{
		std::string error = SDL_GetError();
		SCION_ERROR("Failed to create OpenGL context: {0}", error);
		running = false;
		return -1;
	}

	SDL_GL_MakeCurrent(window.GetWindow().get(), window.GetGLContext());
	SDL_GL_SetSwapInterval(1);

	// Initialize Glad
	if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0)
	{
		SCION_ERROR("Failed to LoadGL --> GLAD");
		running = false;
		return -1;
	}

	// Enable Alpha Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create a registry
	auto pRegistry = std::make_unique<entt::registry>();
	if (!pRegistry)
	{
		SCION_ERROR("Failed to create the entt registry!");
		return -1;
	}


	// Add temp texture
	auto texture = SCION_RENDERING::TextureLoader::Create(SCION_RENDERING::Texture::TextureType::PIXEL, "./assets/textures/castle.png");

	if (!texture)
	{
		SCION_ERROR("Failed to create the texure!");
		return -1;
	}

	// Let's make some temporary UVs
	UVs uVs{};
	SCION_LOG("Loaded Texture: [width = {0}, height = {1}]", texture->GetWidth(), texture->GetHeight());
	SCION_WARN("Loaded Texture: [width = {0}, height = {1}]", texture->GetWidth(), texture->GetHeight());

	// Create a new entity -- for test
	auto ent1 = pRegistry->create();
	
	auto& transform = pRegistry->emplace<TransformComponent>(ent1, TransformComponent{
				.position = glm::vec2{10.f, 10.f},
				.scale = glm::vec2{1.f, 1.f},
				.rotation = 0.f
		}
	);

	auto& sprite = pRegistry->emplace<SpriteComponent>(ent1, SpriteComponent{
				.width = 16.f,
				.height = 16.f,
				.color = SCION_RENDERING::Color{.r = 255, .g = 0, .b = 255, .a = 255},
				.start_x = 0,
				.start_y = 28
		}
	);
	
	sprite.generate_uvs(texture->GetWidth(), texture->GetHeight());

	// These are all test values to be removed	
	std::vector<SCION_RENDERING::Vertex> vertices{};
	SCION_RENDERING::Vertex vTL{}, vTR{}, vBL{}, vBR{};

	vTL.position = glm::vec2{ transform.position.x, transform.position.y + sprite.height};
	vTL.uvs = glm::vec2{ sprite.uvs.u, sprite.uvs.v + sprite.uvs.uv_height};

	vTR.position = glm::vec2{ transform.position.x + sprite.width, transform.position.y + sprite.height};
	vTR.uvs = glm::vec2{ sprite.uvs.u + sprite.uvs.uv_width, sprite.uvs.v + sprite.uvs.uv_height };

	vBL.position = glm::vec2{ transform.position.x, transform.position.y };
	vBL.uvs = glm::vec2{ sprite.uvs.u, sprite.uvs.v};

	vBR.position = glm::vec2{ transform.position.x + sprite.width, transform.position.y};
	vBR.uvs = glm::vec2{ sprite.uvs.u + sprite.uvs.uv_width, sprite.uvs.v};

	vertices.push_back(vTL);
	vertices.push_back(vBL);
	vertices.push_back(vBR);
	vertices.push_back(vTR);

	GLuint indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};
	
	// Create a temp camera
	SCION_RENDERING::Camera2D camera{};
	camera.SetScale(15.f);

	// Create out first shader
	auto shader = SCION_RENDERING::ShaderLoader::Create("assets/shaders/basicShader.vert", "assets/shaders/basicShader.frag");

	if (!shader)
	{
		SCION_ERROR("Failed to create the shader!");
		return -1;
	}

	// Now we will have to create the vertex array object and the vertex buffer object
	GLuint VAO, VBO, IBO;

	// Let's generate the VAO
	glGenVertexArrays(1, &VAO);

	// Generate the VBO
	glGenBuffers(1, &VBO);

	// Bind the VAO and VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(
		GL_ARRAY_BUFFER,										// The target buffer type
		vertices.size() * sizeof(SCION_RENDERING::Vertex),		// The size in bytes of the buffer object's new data store
		vertices.data(),										// A pointer to the data that will be copied into the data store
		GL_STATIC_DRAW											// The expected usage pattern of the data store
	);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,								// The target buffer type
		6 * sizeof(GLuint),										// The size in bytes of the buffer object's new data store
		indices,												// A pointer to the data that will be copied into the data store
		GL_STATIC_DRAW											// The expected usage pattern of the data store
	);

	glVertexAttribPointer(
		0,														// Attribute 0	-- The layout position in the shader
		2,														// Size			-- Number of components per vertex
		GL_FLOAT,												// Type			-- The data type of the above components
		GL_FALSE,												// Normalized	-- Specifies if fixed-point data values should be normalized
		sizeof(SCION_RENDERING::Vertex),						// Stride		-- Specifies the byte offset between consecutive attributes
		(void*)offsetof(SCION_RENDERING::Vertex, position)		// Pointer		-- Specifies the offset of the first component
	);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1, 
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(SCION_RENDERING::Vertex),
		(void*)offsetof(SCION_RENDERING::Vertex, uvs)			// This the offset of the positional data to the first UV coordinate
	);
	
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		2,
		4,
		GL_UNSIGNED_BYTE,
		GL_TRUE,
		sizeof(SCION_RENDERING::Vertex),
		(void*)offsetof(SCION_RENDERING::Vertex, color)			// This the offset of the positional data to the first UV coordinate
	);

	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	SDL_Event event{};

	// Window loop
	while (running)
	{
		// Process Events
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					running = false;
				break;
			default:
				break;
			}
		}

		glViewport(
			0,
			0,
			window.GetWidth(),
			window.GetHeight()
		);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		shader->Enable();
		glBindVertexArray(VAO);

		auto projection = camera.GetCameraMatrix();

		shader->SetUniformMat4("uProjection", projection);

		glActiveTexture(GL_TEXTURE0); 
		glBindTexture(GL_TEXTURE_2D, texture->GetID());


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		SDL_GL_SwapWindow(window.GetWindow().get());

		camera.Update();
		shader->Disable();
	}

	SCION_LOG("Closing!");
	return 0;
}
