#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <memory>

/*
* @brief Simple wrapper for SDL C-style types. 
* @brief These are custom deleters that allow us to use shared pointers to handle the clean up of the SDL types
*/
namespace SCION_UTIL {
	struct SDL_Destroyer
	{
		void operator()(SDL_Window* window) const;
		void operator()(SDL_GameController* controller) const;
		void operator()(Mix_Chunk* chunk) const;
		void operator()(Mix_Music* music) const;
		void operator()(SDL_Cursor* cursor) const;
	};
}

// Useful Aliases
using Controller	= std::shared_ptr<SDL_GameController>;
using Cursor		= std::shared_ptr<SDL_Cursor>;
using WindowPtr		= std::unique_ptr<SDL_Window, SCION_UTIL::SDL_Destroyer>;
using SoundFxPtr	= std::unique_ptr<Mix_Chunk, SCION_UTIL::SDL_Destroyer>;
using MusicPtr		= std::unique_ptr<Mix_Music, SCION_UTIL::SDL_Destroyer>;


static Cursor make_shared_cursor(SDL_Cursor* cursor)
{
	return Cursor();
}

/*
* @brief Creates a shared ptr of the Aliased type Controller.
* @param Takes in a pointer to the SDL_GameController
* @return Returns a newly created shared ptr with the custom deleter.
*/
static Controller make_shared_controller(SDL_GameController* controller)
{
	return std::shared_ptr<SDL_GameController>(controller, SCION_UTIL::SDL_Destroyer{});
}
