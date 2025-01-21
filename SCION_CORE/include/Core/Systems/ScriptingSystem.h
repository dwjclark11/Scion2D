#pragma once
#include <sol/sol.hpp>
#include <string>

namespace SCION_CORE::ECS
{
class Registry;
}

namespace SCION_CORE::Systems
{
class ScriptingSystem
{
  private:
	bool m_bMainLoaded;

  public:
	ScriptingSystem();
	~ScriptingSystem() = default;

	bool LoadMainScript( const std::string& sMainLuaFile, SCION_CORE::ECS::Registry& registry, sol::state& lua );
	void Update( SCION_CORE::ECS::Registry& registry );
	void Render( SCION_CORE::ECS::Registry& registry );

	static void RegisterLuaBindings( sol::state& lua, SCION_CORE::ECS::Registry& registry );
	static void RegisterLuaFunctions( sol::state& lua, SCION_CORE::ECS::Registry& registry );
};

} // namespace SCION_CORE::Systems
