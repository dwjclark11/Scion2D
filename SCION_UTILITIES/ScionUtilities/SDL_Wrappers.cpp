#include "SDL_Wrappers.h"
#include <iostream>

void SCION_UTIL::SDL_Destroyer::operator()(SDL_Window* window) const
{
    SDL_DestroyWindow(window);
    std::cout << "Destroyed SDL WINDOW\n";
}

void SCION_UTIL::SDL_Destroyer::operator()(SDL_GameController* controller) const
{

}

void SCION_UTIL::SDL_Destroyer::operator()(SDL_Cursor* cursor) const
{

}

Controller make_shared_controller(SDL_GameController* controller)
{
    return Controller();
}

Cursor make_shared_cursor(SDL_Cursor* cursor)
{
    return Cursor();
}
