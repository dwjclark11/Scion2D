#ifdef SCION_OPENGL_FORCE_DISCRETE_GPU
	#ifdef _WIN32
	extern "C"
	{
		__declspec( dllexport ) unsigned long NvOptimusEnablement = 0x00000001;
		__declspec( dllexport ) int AmdPowerXpressRequestHighPerformance = 1;
	}
	#endif //!_WIN32
#endif // !SCION_OPENGL_FORCE_DISCRETE_GPU

#define SDL_MAIN_HANDLED 1

#include "Application.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
#ifndef NDEBUG
#ifdef _WIN32
	ShowWindow( GetConsoleWindow(), SW_SHOW );
#else
	// ADD Linux stuff
#endif
#else
#ifdef _WIN32
	ShowWindow( GetConsoleWindow(), SW_HIDE );
#else
	// ADD Linux stuff
#endif
#endif
#if __linux
	#if defined SCION_OPENGL_FORCE_DISCRETE_GPU
		// TODO: Routine to offload AMD discrete GPU
		std::string NV_VENDOR = "nvidia";
		std::string ENV_NV_GL_RENDER_OFFLOAD = "__NV_PRIME_RENDER_OFFLOAD=1";
		std::string ENV_NV_GL_RENDER_LIBRARY = "__GLX_VENDOR_LIBRARY_NAME=" + NV_VENDOR;
		
		::putenv(ENV_NV_GL_RENDER_OFFLOAD.data());
		::putenv(ENV_NV_GL_RENDER_LIBRARY.data());
	#endif // !SCION_OPENGL_FORCE_DISCRETE_GPU
#endif // !__linux

	auto& app = SCION_EDITOR::Application::GetInstance();
	app.Run();

	return 0;
}
