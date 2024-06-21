#define SDL_MAIN_HANDLED 1

#include "Application.h"

int main()
{
#ifndef NDEBUG
	// TODO: SHOW CONSOLE via OS
#else
	// TODO: HIDE CONSOLE via OS
#endif

	auto& app = SCION_EDITOR::Application::GetInstance();
	app.Run();

	return 0;
}
