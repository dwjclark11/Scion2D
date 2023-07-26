#define SDL_MAIN_HANDLED 1;
#include <Windowing/Window/Window.h>
#include <SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Rendering/Essentials/ShaderLoader.h>

#include <Logger/Logger.h>


class Camera2D
{
private:
	int m_Width, m_Height;
	float m_Scale;

	glm::vec2 m_Position;
	glm::mat4 m_CameraMatrix, m_OrthoProjection;

	bool m_bNeedsUpdate;

public:
	Camera2D()
		: Camera2D(640, 480)
	{

	}

	Camera2D(int width, int height)
		: m_Width{width}, m_Height{height}, m_Scale{1.f}
	, m_Position{ glm::vec2{0} }, m_CameraMatrix{ 1.f }, m_OrthoProjection{ 1.f }, m_bNeedsUpdate{ true }
	{
		// Init ortho projection
		m_OrthoProjection = glm::ortho(
			0.f,							// Left
			static_cast<float>(m_Width),	// Right
			static_cast<float>(m_Height),	// Top
			0.f,							// Bottom
			0.f,							// Near
			1.f								// Far
		);
	}
	
	inline void SetScale(float scale) { m_Scale = scale; m_bNeedsUpdate = true; }

	inline glm::mat4 GetCameraMatrix() { return m_CameraMatrix; }
	
	void Update()
	{
		if (!m_bNeedsUpdate)
			return;
		// Translate
		glm::vec3 translate{ -m_Position.x, -m_Position.y, 0.f };
		m_CameraMatrix = glm::translate(m_OrthoProjection, translate);

		// Scale
		glm::vec3 scale{ m_Scale, m_Scale, 0.f };
		m_CameraMatrix *= glm::scale(glm::mat4(1.f), scale);

		m_bNeedsUpdate = false;
	}
};

struct UVs
{
	float u, v, width, height;
	UVs()
		: u{ 0.f }, v{ 0.f }, width{ 0.f }, height{ 0.f }
	{

	}
};



bool LoadTexture(const std::string& filepath, int& width, int& height, bool blended)
{
	int channels = 0;

	unsigned char* image = SOIL_load_image(
		filepath.c_str(),				// Filename			-- Image file to be loaded	
		&width,							// Width			-- Width of the image
		&height,						// height			-- Height of the image
		&channels,						// channels			-- Number of channels
		SOIL_LOAD_AUTO					// force_channels	-- Force the channels count
	);

	// Check to see if the image is successful
	if (!image)
	{
		std::cout << "SOIL failed to load image [" << filepath << "] -- " << SOIL_last_result() << std::endl;
		return false;
	}

	GLint format = GL_RGBA;

	switch (channels)
	{
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (!blended)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	glTexImage2D(
		GL_TEXTURE_2D,			// target			-- Specifies the target texture
		0,						// level			-- Level of detail. 0 is the base image level
		format,					// internal format	-- The number of color components
		width,					// width			-- width of the texture image
		height,					// height			-- height of the texture image
		0,						// border
		format,					// format			-- format of the pixel data
		GL_UNSIGNED_BYTE,		// type				-- The data type of the pixel data
		image					// data
	);

	// Delete the image data from SOIL
	SOIL_free_image_data(image);

	return true;
}


int main()
{
	SCION_INIT_LOGS(true, true);

	bool running{ true };

	// Init SDL
	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::string error = SDL_GetError();
		std::cout << "Failed to initialize SDL: " << error << std::endl;
		running = false;
		return -1;
	}

	// Set up OpenGL
	if (SDL_GL_LoadLibrary(NULL) != 0)
	{
		std::string error = SDL_GetError();
		std::cout << "Failed to Open GL Library: " << error << std::endl;
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
		std::cout << "Failed to create the window!" << std::endl;
		return -1;
	}

	// Create the openGL context
	window.SetGLContext(SDL_GL_CreateContext(window.GetWindow().get()));

	if (!window.GetGLContext())
	{
		std::string error = SDL_GetError();
		std::cout << "Failed to create OpenGL context: " << error << "\n";
		running = false;
		return -1;
	}

	SDL_GL_MakeCurrent(window.GetWindow().get(), window.GetGLContext());
	SDL_GL_SetSwapInterval(1);

	// Initialize Glad
	if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0)
	{
		std::cout << "Failed to LoadGL --> GLAD" << std::endl;
		running = false;
		return -1;
	}

	// Enable Alpha Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Temporarily load a texture
	// First create the texture ID and gen/bind the texture
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// create width and height for the texture
	int width{ 0 }, height{ 0 };

	// Now we can load the texture
	if (!LoadTexture("assets/textures/castle.png", width, height, false))
	{
		SCION_ERROR("Failed to load the texture!");
		return -1;
	}

	// Let's make some temporary UVs
	UVs uvs{};
	SCION_LOG("Loaded Texture: [width = {0}, height = {1}]", width, height);
	SCION_WARN("Loaded Texture: [width = {0}, height ={1}]", width, height);
	auto generateUVs = [&](float startX, float startY, float spriteWidth, float spriteHeight)
	{
		uvs.width = spriteWidth / width;
		uvs.height = spriteHeight / height;

		uvs.u = startX * uvs.width;
		uvs.v = startY * uvs.height;
	};

	generateUVs(0, 28, 16, 16);

	// Create vertices for a Quad
	//float vertices[] =
	//{
	//	-0.5f, 0.5f, 0.0f,	0.f, 1.f,	// TL
	//	0.5f, 0.5f, 0.0f,	1.f, 1.f,	// TR
	//	0.5f, -0.5f, 0.0f,  1.f, 0.f,	// BR
	//	-0.5f, -0.5f, 0.0f, 0.f, 0.f	// BL
	//};
	
	// Swapped tex coords
	float vertices[] =
	{
		10.f, 26.f, 0.0f,	uvs.u, (uvs.v + uvs.height),					// TL
		10.f, 10.f, 0.0f,	uvs.u, uvs.v,									// BL
		26.f, 10.f, 0.0f,  (uvs.u + uvs.width), uvs.v,						// BR
		26.f, 26.f, 0.0f, (uvs.u + uvs.width), (uvs.v + uvs.height)			// TR
	};


	GLuint indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};
	
	// Create a temp camera
	Camera2D camera{};
	camera.SetScale(5.f);

	// Create out first shader
	auto shader = SCION_RENDERING::ShaderLoader::Create("assets/shaders/basicShader.vert", "assets/shaders/basicShader.frag");

	if (!shader)
	{
		std::cout << "Failed to create the shader!" << std::endl;
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
		GL_ARRAY_BUFFER,						// The target buffer type
		sizeof(vertices) * 3 * sizeof(float),	// The size in bytes of the buffer object's new data store
		vertices,								// A pointer to the data that will be copied into the data store
		GL_STATIC_DRAW							// The expected usage pattern of the data store
	);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,				// The target buffer type
		6 * sizeof(GLuint),	// The size in bytes of the buffer object's new data store
		indices,								// A pointer to the data that will be copied into the data store
		GL_STATIC_DRAW							// The expected usage pattern of the data store
	);

	glVertexAttribPointer(
		0,										// Attribute 0	-- The layout position in the shader
		3,										// Size			-- Number of components per vertex
		GL_FLOAT,								// Type			-- The data type of the above components
		GL_FALSE,								// Normalized	-- Specifies if fixed-point data values should be normalized
		5 * sizeof(float),						// Stride		-- Specifies the byte offset between consecutive attributes
		(void*)0								// Pointer		-- Specifies the offset of the first component
	);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1, 
		2,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(float),
		reinterpret_cast<void*>(sizeof(float) * 3)		// This the offset of the positional data to the first UV coordinate
	);
	
	glEnableVertexAttribArray(1);

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
		glBindTexture(GL_TEXTURE_2D, texID);


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		SDL_GL_SwapWindow(window.GetWindow().get());

		camera.Update();
		shader->Disable();
	}

	std::cout << "Closing!" << std::endl;
	return 0;
}