#include "SDL_Wrappers.h"
#include <iostream>

void SCION_UTIL::SDL_Destroyer::operator()(SDL_Window* window) const
{
    SDL_DestroyWindow(window);
    std::cout << "Destroyed SDL WINDOW\n";
}

void SCION_UTIL::SDL_Destroyer::operator()(SDL_GameController* controller) const
{
    SDL_GameControllerClose(controller);
    controller = nullptr;
    std::cout << "Closed SDL Game Controller!\n";
}

void SCION_UTIL::SDL_Destroyer::operator()(SDL_Cursor* cursor) const
{

}

Cursor make_shared_cursor(SDL_Cursor* cursor)
{
    return Cursor();
}
