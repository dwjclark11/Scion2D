#define SDL_MAIN_HANDLED 1
#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

static std::string LoadCrashLog( const char* path )
{
	std::ifstream file{ path };
	if ( !file )
	{
		return std::string{ "Could not open crash log at:\n" } + path;
	}

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

static std::string GetLastCrashEntry( const char* path )
{
	std::ifstream file{ path };
	if ( !file )
	{
		return std::string{ "Could not open crash log at:\n" } + path;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string sContent = buffer.str();

	std::vector<size_t> positions;
	std::string separator{ "[CRITICAL] Program crashed!" };
	size_t pos = sContent.find( separator );
	while ( pos != std::string::npos )
	{
		positions.push_back( pos );
		pos = sContent.find( separator, pos + separator.length() );
	}

	if ( positions.empty() )
	{
		return std::string{ "No crash entries were found." };
	}

	// Only one separator = last log goes to EOF
	if ( positions.size() == 1 )
		return sContent.substr( positions[ 0 ] );

	// Two or more separators = return content from last to EOF
	size_t last = positions.back();
	size_t next = sContent.find( separator, last + separator.length() );

	return sContent.substr( last, next == std::string::npos ? std::string::npos : next - last );
	/*size_t lastPos = positions.back();
	return sContent.substr( lastPos );*/
}

int main( int argc, char* argv[] )
{

#ifdef _WIN32
#ifdef NDEBUG
	ShowWindow( GetConsoleWindow(), SW_HIDE );
#else
	ShowWindow( GetConsoleWindow(), SW_SHOW );
#endif // NDEBUG
	MessageBeep( MB_ICONERROR );
#endif // WIN32

	if ( argc < 2 )
	{
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Crash Reporter", "No crash log path provided.", nullptr );

		return 1;
	}

	const char* sCrashLogPath = argv[ 1 ];
	// std::string sCrashText = LoadCrashLog( sCrashLogPath );
	std::string sCrashText = GetLastCrashEntry( sCrashLogPath );

	// SDL Init
	if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) != 0 )
	{
		return 1;
	}

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );

	SDL_Window* pWindow = SDL_CreateWindow(
		"Crash Reporter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL );

	SDL_GLContext glContext = SDL_GL_CreateContext( pWindow );
	SDL_GL_MakeCurrent( pWindow, glContext );
	SDL_GL_SetSwapInterval( 1 );

	// Initialize Glad
	if ( gladLoadGLLoader( SDL_GL_GetProcAddress ) == 0 )
	{
		return 1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL( pWindow, glContext );
	ImGui_ImplOpenGL3_Init( "#version 330" );

	bool bDone{ false };

	while ( !bDone )
	{
		SDL_Event event;
		while ( SDL_PollEvent( &event ) )
		{
			ImGui_ImplSDL2_ProcessEvent( &event );
			if ( event.type == SDL_QUIT )
				bDone = true;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos( ImVec2{ 0.f, 0.f } );
		ImGui::SetNextWindowSize( io.DisplaySize );

		if ( ImGui::Begin( "Crash Reporter",
						   nullptr,
						   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
							   ImGuiWindowFlags_NoTitleBar ) )
		{
			ImGui::TextWrapped( "A crash occured. Please see the log below and report it if needed:" );

			ImGui::BeginChild( "ScrollRegion", ImVec2{ 0.f, -40.f }, true, ImGuiWindowFlags_HorizontalScrollbar );
			ImGui::TextUnformatted( sCrashText.c_str() );
			ImGui::EndChild();

			if ( ImGui::Button( "OK" ) )
			{
				bDone = true;
			}

			ImGui::End();
		}

		ImGui::Render();
		glViewport( 0, 0, static_cast<int>( io.DisplaySize.x ), static_cast<int>( io.DisplaySize.y ) );
		glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );
		ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
		SDL_GL_SwapWindow( pWindow );
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext( glContext );
	SDL_DestroyWindow( pWindow );
	SDL_Quit();

	return 0;
}
