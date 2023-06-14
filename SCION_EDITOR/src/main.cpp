#define SDL_MAIN_HANDLED 1;
#include <Windowing/Window/Window.h>
#include <SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <SOIL/SOIL.h>

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
		std::cout << "SOIL failed to load image [" << filepath << "] -- " << SOIL_last_result();
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
	SCION_WINDOWING::Window window("Test Window", 480, 480, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, true, SDL_WINDOW_OPENGL);

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
		std::cout << "Failed to load the texture!\n";
		return -1;
	}


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
		-0.5f, 0.5f, 0.0f,	0.f, 0.f,	// TL
		0.5f, 0.5f, 0.0f,	1.f, 0.f,	// TR
		0.5f, -0.5f, 0.0f,  1.f, 1.f,	// BR
		-0.5f, -0.5f, 0.0f, 0.f, 1.f	// BL
	};


	GLuint indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};
	
	// Create a temp vertex source
	const char* vertexSource =
		"#version 450 core\n"
		"layout (location = 0) in vec3 aPosition;\n"
		"layout (location = 1) in vec2 aTexCoords;\n"
		"out vec2 fragUVs;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(aPosition, 1.0);\n"
		"    fragUVs = aTexCoords;\n"
		"}\0";

	// Create the shader
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Add the vertex shader source
	glShaderSource(vertexShader, 1, &vertexSource, NULL);

	// Compile the vertex shader
	glCompileShader(vertexShader);

	// Get the comilation status
	int status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Failed to compile vertex shader!\n" << infoLog << std::endl;
		return -1;
	}

	// Create a temp fragment shader
	const char* fragmentSource =
		"#version 450 core\n"
		"in vec2 fragUVs;\n"
		"out vec4 color;\n"
		"uniform sampler2D uTexture;\n"
		"void main()\n"
		"{\n"
		//"    color = vec4(1.0f, 0.0f, 1.0f, 1.0f);\n"
		"    color = texture(uTexture, fragUVs);\n"
		"}\0";

	// Create the shader
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Add the fragment shader source
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

	// Compile the fragment shader
	glCompileShader(fragmentShader);

	// Get the comilation status
	status;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "Failed to compile fragment shader!\n" << infoLog << std::endl;
		return -1;
	}

	// Create the shader program
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();

	// Attach the above shaders to the program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Now we want to link the program
	glLinkProgram(shaderProgram);

	// Check the link status
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);

	if (!status)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Failed to link shader program!\n" << infoLog << std::endl;
		return -1;
	}

	// Now we can enable the shader program
	glUseProgram(shaderProgram);

	// Once the program is created and linked, you can delete the shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		glActiveTexture(GL_TEXTURE0); 
		glBindTexture(GL_TEXTURE_2D, texID);


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		SDL_GL_SwapWindow(window.GetWindow().get());
	}

	std::cout << "Closing!" << std::endl;
	return 0;
}