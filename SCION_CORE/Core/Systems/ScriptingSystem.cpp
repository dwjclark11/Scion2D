#include "ScriptingSystem.h"
#include "../ECS/Components/ScriptComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Components/SpriteComponent.h"
#include "../ECS/Entity.h"

#include "../Scripting/GlmLuaBindings.h"

#include <Logger/Logger.h>

using namespace SCION_CORE::ECS;

namespace SCION_CORE::Systems {

	ScriptingSystem::ScriptingSystem(SCION_CORE::ECS::Registry& registry)
		: m_Registry(registry), m_bMainLoaded{ false }
	{
	}

	bool ScriptingSystem::LoadMainScript(sol::state& lua)
	{
		try
		{
			auto result = lua.safe_script_file("./assets/scripts/main.lua");
		}
		catch (const sol::error& err)
		{
			SCION_ERROR("Error loading the main lua script: {0}", err.what());
			return false;
		}

		sol::table main_lua = lua["main"];
		sol::optional<sol::table> bUpdateExists = main_lua[1];
		if (bUpdateExists == sol::nullopt)
		{
			SCION_ERROR("There is no update function in main.lua");
			return false;
		}

		sol::table update_script = main_lua[1];
		sol::function update = update_script["update"];

		sol::optional<sol::table> bRenderExists = main_lua[2];
		if (bRenderExists == sol::nullopt)
		{
			SCION_ERROR("There is no render function in main.lua");
			return false;
		}

		sol::table render_script = main_lua[2];
		sol::function render = render_script["render"];

		SCION_CORE::ECS::Entity mainLuaScript{m_Registry, "main_script", ""};
		mainLuaScript.AddComponent<SCION_CORE::ECS::ScriptComponent>(
			SCION_CORE::ECS::ScriptComponent{
				.update = update,
				.render = render
			}
		);

		m_bMainLoaded = true;
		return true;
	}

	void ScriptingSystem::Update()
	{
		if (!m_bMainLoaded)
		{
			SCION_ERROR("Main lua script has not been loaded!");
			return;
		}

		auto view = m_Registry.GetRegistry().view<SCION_CORE::ECS::ScriptComponent>();

		for (const auto& entity : view)
		{
			SCION_CORE::ECS::Entity ent{m_Registry, entity};
			if (ent.GetName() != "main_script")
				continue;

			auto& script = ent.GetComponent<SCION_CORE::ECS::ScriptComponent>();
			auto error = script.update(entity);
			if (!error.valid())
			{
				sol::error err = error;
				SCION_ERROR("Error running the Update script: {0}", err.what());
			}
		}
	}

	void ScriptingSystem::Render()
	{
		if (!m_bMainLoaded)
		{
			SCION_ERROR("Main lua script has not been loaded!");
			return;
		}

		auto view = m_Registry.GetRegistry().view<SCION_CORE::ECS::ScriptComponent>();

		for (const auto& entity : view)
		{
			SCION_CORE::ECS::Entity ent{m_Registry, entity};
			if (ent.GetName() != "main_script")
				continue;

			auto& script = ent.GetComponent<SCION_CORE::ECS::ScriptComponent>();
			auto error = script.render(entity);
			if (!error.valid())
			{
				sol::error err = error;
				SCION_ERROR("Error running the Render script: {0}", err.what());
			}
		}
	}

	void ScriptingSystem::RegisterLuaBindings(sol::state& lua, SCION_CORE::ECS::Registry& registry)
	{
		SCION_CORE::Scripting::GLMBindings::CreateGLMBindings(lua);

		Registry::CreateLuaRegistryBind(lua, registry);
		Entity::CreateLuaEntityBind(lua, registry);
		TransformComponent::CreateLuaTransformBind(lua);
		SpriteComponent::CreateSpriteLuaBind(lua, registry);

		Entity::RegisterMetaComponent<TransformComponent>();
		Entity::RegisterMetaComponent<SpriteComponent>();
		
		Registry::RegisterMetaComponent<TransformComponent>();
		Registry::RegisterMetaComponent<SpriteComponent>();

	}
}