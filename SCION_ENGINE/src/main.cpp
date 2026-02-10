#define SDL_MAIN_HANDLED 1
#include "Runtime.h"


#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{

#ifdef _WIN32
#ifdef NDEBUG
	ShowWindow( GetConsoleWindow(), SW_HIDE );
#else
	ShowWindow( GetConsoleWindow(), SW_SHOW );
#endif // NDEBUG
#endif // _WIN32
	Scion::Engine::RuntimeApp app{};
	app.Run();

	return 0;
}
