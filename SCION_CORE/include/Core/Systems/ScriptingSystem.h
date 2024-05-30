#pragma once
#include "Core/ECS/Registry.h"
#include <sol/sol.hpp>

namespace SCION_CORE::Systems
{
class ScriptingSystem
{
  private:
	SCION_CORE::ECS::Registry& m_Registry;
	bool m_bMainLoaded;

  public:
	ScriptingSystem(SCION_CORE::ECS::Registry& registry);
	~ScriptingSystem() = default;

	bool LoadMainScript(sol::state& lua);
	void Update();
	void Render();

	static void RegisterLuaBindings(sol::state& lua, SCION_CORE::ECS::Registry& registry);
	static void RegisterLuaFunctions(sol::state& lua, SCION_CORE::ECS::Registry& registry);
};

} // namespace SCION_CORE::Systems