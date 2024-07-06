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

	auto& app = SCION_EDITOR::Application::GetInstance();
	app.Run();

	return 0;
}
