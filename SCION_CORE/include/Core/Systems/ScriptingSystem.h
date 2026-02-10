#pragma once
#include <sol/sol.hpp>

namespace Scion::Core
{

struct ProjectInfo;

namespace ECS
{
class Registry;
}
} // namespace Scion::Core

namespace Scion::Core::Systems
{
class ScriptingSystem
{
  public:
	ScriptingSystem();
	~ScriptingSystem() = default;

	bool LoadMainScript( const std::string& sMainLuaFile, Scion::Core::ECS::Registry& registry, sol::state& lua );
	bool LoadMainScript( Scion::Core::ProjectInfo& projectInfo, Scion::Core::ECS::Registry& registry, sol::state& lua );

	void Update( Scion::Core::ECS::Registry& registry );
	void Render( Scion::Core::ECS::Registry& registry );

	static void RegisterLuaBindings( sol::state& lua, Scion::Core::ECS::Registry& registry );
	static void RegisterLuaFunctions( sol::state& lua, Scion::Core::ECS::Registry& registry );
	static void RegisterLuaEvents( sol::state& lua, Scion::Core::ECS::Registry& registry );
	static void RegisterLuaSystems( sol::state& lua, Scion::Core::ECS::Registry& registry );

  private:
	bool m_bMainLoaded;
};

} // namespace Scion::Core::Systems
