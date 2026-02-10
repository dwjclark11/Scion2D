#pragma once
#include <sol/sol.hpp>
namespace Scion::Core
{
namespace ECS
{
class Registry;
}
} // namespace Scion::Core

namespace Scion::Core::Scripting
{
/*
 * @brief Binds the MusicPlayer and the SoundPlayer to Lua using Sol2.
 */
struct SoundBinder
{
	static void CreateSoundBind( sol::state& lua );
};
} // namespace Scion::Core::Scripting
