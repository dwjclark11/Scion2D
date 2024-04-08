#include "SDL_Wrappers.h"
#include <iostream>
#include <Logger/Logger.h>

void SCION_UTIL::SDL_Destroyer::operator()(SDL_Window* window) const
{
    SDL_DestroyWindow(window);
    SCION_LOG("Destroyed SDL WINDOW");
}

void SCION_UTIL::SDL_Destroyer::operator()(SDL_GameController* controller) const
{
    SDL_GameControllerClose(controller);
    controller = nullptr;
    SCION_LOG("Closed SDL Game Controller!");
}

void SCION_UTIL::SDL_Destroyer::operator()(Mix_Chunk* chunk) const
{
    Mix_FreeChunk(chunk);
    SCION_LOG("Freed SDL Mix_Chunk!");
}

void SCION_UTIL::SDL_Destroyer::operator()(Mix_Music* music) const
{
    Mix_FreeMusic(music);
    SCION_LOG("Freed SDL Mix_Chunk!");
}

void SCION_UTIL::SDL_Destroyer::operator()(SDL_Cursor* cursor) const
{

}

