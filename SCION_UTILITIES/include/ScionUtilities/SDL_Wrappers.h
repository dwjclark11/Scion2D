#pragma once
#include <SDL.h>
#include <SDL_mixer.h>

namespace SCION_UTIL
{
/*
 * @brief Simple wrapper for SDL C-style types.
 * @brief These are custom deleters that allow us to use shared pointers to
 * handle the clean up of the SDL types
 */
struct SDL_Destroyer
{
	void operator()( SDL_Window* window ) const;
	void operator()( SDL_GameController* controller ) const;
	void operator()( Mix_Chunk* chunk ) const;
	void operator()( Mix_Music* music ) const;
	void operator()( SDL_Cursor* cursor ) const;
};
} // namespace SCION_UTIL

// Useful Aliases
using Controller = std::shared_ptr<SDL_GameController>;
using Cursor = std::shared_ptr<SDL_Cursor>;
using WindowPtr = std::unique_ptr<SDL_Window, SCION_UTIL::SDL_Destroyer>;
using SoundFxPtr = std::unique_ptr<Mix_Chunk, SCION_UTIL::SDL_Destroyer>;
using MusicPtr = std::unique_ptr<Mix_Music, SCION_UTIL::SDL_Destroyer>;

/**
 * @brief Creates a std::shared_ptr from a raw SDL pointer with a custom deleter.
 *
 * This utility function wraps a raw SDL pointer in a std::shared_ptr using the
 * SCION_UTIL::SDL_Destroyer as the deleter. This ensures the SDL resource is
 * properly released when the shared pointer goes out of scope.
 *
 * @tparam RPtr      The return pointer type, typically deduced as std::shared_ptr<TSDLType>.
 * @tparam TSDLType  The type of the raw SDL resource.
 * @param pSDLType   The raw pointer to the SDL resource to manage.
 * @return A std::shared_ptr managing the SDL resource with a custom deleter.
 */
template <typename RPtr, typename TSDLType>
inline RPtr MakeSharedFromSDLType( TSDLType* pSDLType )
{
	return std::shared_ptr<TSDLType>( pSDLType, SCION_UTIL::SDL_Destroyer{} );
}
