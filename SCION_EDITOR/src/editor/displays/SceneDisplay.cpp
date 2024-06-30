#include "SceneDisplay.h"
#include "Rendering/Buffers/Framebuffer.h"
#include "Rendering/Core/Camera2D.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Systems/AnimationSystem.h"
#include "Core/Systems/PhysicsSystem.h"
#include "Core/Systems/ScriptingSystem.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Logger/Logger.h"
#include "Sounds/MusicPlayer/MusicPlayer.h"
#include "Sounds/SoundPlayer/SoundFxPlayer.h"
#include "Physics/Box2DWrappers.h"
#include "Core/Resources/AssetManager.h"
#include <imgui.h>

using namespace SCION_CORE::Systems;

constexpr float one_over_sixty = 1.f / 60.f;

namespace SCION_EDITOR
{

void SceneDisplay::LoadScene()
{
	auto& scriptSystem = m_Registry.GetContext<std::shared_ptr<ScriptingSystem>>();
	auto& lua = m_Registry.GetContext<std::shared_ptr<sol::state>>();

	if ( !lua )
		lua = std::make_shared<sol::state>();

	lua->open_libraries( sol::lib::base,
						 sol::lib::math,
						 sol::lib::os,
						 sol::lib::table,
						 sol::lib::io,
						 sol::lib::string,
						 sol::lib::package );

	SCION_CORE::Systems::ScriptingSystem::RegisterLuaBindings( *lua, m_Registry );
	SCION_CORE::Systems::ScriptingSystem::RegisterLuaFunctions( *lua, m_Registry );

	if ( !scriptSystem->LoadMainScript( *lua ) )
	{
		SCION_ERROR( "Failed to load the main lua script!" );
		return;
	}

	m_bSceneLoaded = true;
	m_bPlayScene = true;
}

void SceneDisplay::UnloadScene()
{
	m_bPlayScene = false;
	m_bSceneLoaded = false;
	m_Registry.GetRegistry().clear();
	auto& lua = m_Registry.GetContext<std::shared_ptr<sol::state>>();
	lua.reset();

	auto& mainRegistry = MAIN_REGISTRY();
	mainRegistry.GetMusicPlayer().Stop();
	mainRegistry.GetSoundPlayer().Stop( -1 );
}

SceneDisplay::SceneDisplay( SCION_CORE::ECS::Registry& registry )
	: m_Registry{ registry }
	, m_bPlayScene{ false }
	, m_bSceneLoaded{ false }
{
}

void SceneDisplay::Draw()
{
	static bool pOpen{ true };
	if ( !ImGui::Begin( "Scene", &pOpen ) )
	{
		ImGui::End();
		return;
	}

	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	auto pPlayTexture = assetManager.GetTexture( "play_button" );
	auto pStopTexture = assetManager.GetTexture( "stop_button" );

	if (m_bPlayScene)
	{
		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4{ 0.f, 0.9f, 0.f, 0.3 } );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4{ 0.f, 0.9f, 0.f, 0.3 } );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4{ 0.f, 0.9f, 0.f, 0.3 } );
	}

	if ( ImGui::ImageButton( (ImTextureID)pPlayTexture->GetID(),
							 ImVec2{
								 (float)pPlayTexture->GetWidth() * 0.25f,
								 (float)pPlayTexture->GetHeight() * 0.25f,
							 } ) &&
		 !m_bSceneLoaded )
	{
		LoadScene();
	}

	if ( ImGui::GetColorStackSize() > 0 )
		ImGui::PopStyleColor( ImGui::GetColorStackSize() );

	if ( ImGui::IsItemHovered( ImGuiHoveredFlags_DelayNormal ) )
		ImGui::SetTooltip( "Play Scene" );

	ImGui::SameLine();

	if ( !m_bPlayScene )
	{
		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4{ 0.f, 0.9f, 0.f, 0.3 } );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4{ 0.f, 0.9f, 0.f, 0.3 } );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4{ 0.f, 0.9f, 0.f, 0.3 } );
	}

	if ( ImGui::ImageButton( (ImTextureID)pStopTexture->GetID(),
							 ImVec2{
								 (float)pStopTexture->GetWidth() * 0.25f,
								 (float)pStopTexture->GetHeight() * 0.25f,
							 } ) &&
		 m_bSceneLoaded )
	{
		UnloadScene();
	}

	if ( ImGui::GetColorStackSize() > 0 )
		ImGui::PopStyleColor( ImGui::GetColorStackSize() );

	if ( ImGui::IsItemHovered( ImGuiHoveredFlags_DelayNormal ) )
		ImGui::SetTooltip( "Stop Scene" );

	if ( ImGui::BeginChild( "##SceneChild", ImVec2{ 0.f, 0.f }, NULL, ImGuiWindowFlags_NoScrollWithMouse ) )
	{
		const auto& fb = m_Registry.GetContext<std::shared_ptr<SCION_RENDERING::Framebuffer>>();

		ImGui::SetCursorPos( ImVec2{ 0.f, 0.f } );

		ImGui::Image( (ImTextureID)fb->GetTextureID(),
					  ImVec2{ static_cast<float>( fb->Width() ), static_cast<float>( fb->Height() ) },
					  ImVec2{ 0.f, 1.f },
					  ImVec2{ 1.f, 0.f } );
		ImGui::EndChild();

		// Check for resize based on the window size
		ImVec2 windowSize{ ImGui::GetWindowSize() };
		if ( fb->Width() != static_cast<int>( windowSize.x ) || fb->Height() != static_cast<int>( windowSize.y ) )
			fb->Resize( static_cast<int>( windowSize.x ), static_cast<int>( windowSize.y ) );
	}

	ImGui::End();
}

void SceneDisplay::Update()
{
	if ( !m_bPlayScene )
		return;

	auto& mainRegistry = MAIN_REGISTRY();
	auto& coreGlobals = CORE_GLOBALS();

	auto& camera = m_Registry.GetContext<std::shared_ptr<SCION_RENDERING::Camera2D>>();
	if ( !camera )
	{
		SCION_ERROR( "Failed to get the camera from the registry context!" );
		return;
	}

	camera->Update();

	auto& scriptSystem = m_Registry.GetContext<std::shared_ptr<SCION_CORE::Systems::ScriptingSystem>>();
	scriptSystem->Update();

	if ( coreGlobals.IsPhysicsEnabled() )
	{
		auto& pPhysicsWorld = m_Registry.GetContext<SCION_PHYSICS::PhysicsWorld>();
		pPhysicsWorld->Step( one_over_sixty, coreGlobals.GetVelocityIterations(), coreGlobals.GetPositionIterations() );
		pPhysicsWorld->ClearForces();
	}

	auto& pPhysicsSystem = m_Registry.GetContext<std::shared_ptr<SCION_CORE::Systems::PhysicsSystem>>();
	pPhysicsSystem->Update( m_Registry.GetRegistry() );

	auto& animationSystem = m_Registry.GetContext<std::shared_ptr<SCION_CORE::Systems::AnimationSystem>>();
	animationSystem->Update();
}
} // namespace SCION_EDITOR
