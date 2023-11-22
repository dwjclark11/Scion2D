#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <memory>
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

typedef std::shared_ptr<SDL_GameController> Controller;
static Controller make_shared_controller(SDL_GameController* controller)
{
	return std::shared_ptr<SDL_GameController>(controller, SCION_UTIL::SDL_Destroyer{});
}

typedef std::shared_ptr<SDL_Cursor> Cursor;
static Cursor make_shared_cursor(SDL_Cursor* cursor);

typedef std::unique_ptr<SDL_Window, SCION_UTIL::SDL_Destroyer>		WindowPtr;

typedef std::unique_ptr<Mix_Chunk, SCION_UTIL::SDL_Destroyer>		SoundFxPtr;
typedef std::unique_ptr<Mix_Music, SCION_UTIL::SDL_Destroyer>		MusicPtr;