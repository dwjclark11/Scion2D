#pragma once
#include <sol/sol.hpp>
namespace SCION_CORE
{
namespace ECS
{
class Registry;
}
} // namespace SCION_CORE

namespace SCION_CORE::Scripting
{
/*
 * @brief Binds the MusicPlayer and the SoundPlayer to Lua using Sol2.
 */
struct SoundBinder
{
	static void CreateSoundBind(sol::state& lua, SCION_CORE::ECS::Registry& registry);
};
} // namespace SCION_CORE::Scripting