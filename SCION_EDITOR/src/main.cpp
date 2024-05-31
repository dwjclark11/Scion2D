#define SDL_MAIN_HANDLED 1

#include "Application.h"

int main()
{
	auto& app = SCION_EDITOR::Application::GetInstance();
	app.Run();

	return 0;
}
