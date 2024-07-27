#include "SceneDisplay.h"
#include "Rendering/Buffers/Framebuffer.h"
#include "Rendering/Core/Camera2D.h"
#include "Rendering/Core/Renderer.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Systems/AnimationSystem.h"
#include "Core/Systems/RenderSystem.h"
#include "Core/Systems/RenderUISystem.h"
#include "Core/Systems/RenderShapeSystem.h"
#include "Core/Systems/PhysicsSystem.h"
#include "Core/Systems/ScriptingSystem.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Logger/Logger.h"
#include "Sounds/MusicPlayer/MusicPlayer.h"
#include "Sounds/SoundPlayer/SoundFxPlayer.h"
#include "Physics/Box2DWrappers.h"
#include "Physics/ContactListener.h"
#include "Core/Resources/AssetManager.h"
#include "editor/utilities/EditorFramebuffers.h"

#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"

#include <imgui.h>

using namespace SCION_CORE::Systems;
using namespace SCION_RENDERING;

constexpr float one_over_sixty = 1.f / 60.f;

namespace SCION_EDITOR
{

void SceneDisplay::LoadScene()
{
	auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
	if ( !pCurrentScene )
		return;

	auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();

	const auto& canvas = pCurrentScene->GetCanvas();
	runtimeRegistry.AddToContext<std::shared_ptr<Camera2D>>(
		std::make_shared<Camera2D>( canvas.width, canvas.height ) );

	auto pPhysicsWorld =
		runtimeRegistry.AddToContext<SCION_PHYSICS::PhysicsWorld>( std::make_shared<b2World>( b2Vec2{ 0.f, 9.8f } ) );

	auto pContactListener = runtimeRegistry.AddToContext<std::shared_ptr<SCION_PHYSICS::ContactListener>>(
		std::make_shared<SCION_PHYSICS::ContactListener>() );

	pPhysicsWorld->SetContactListener( pContactListener.get() );

	// Add necessary systems
	auto scriptSystem = runtimeRegistry.AddToContext<std::shared_ptr<ScriptingSystem>>(
		std::make_shared<ScriptingSystem>( runtimeRegistry ) );

	runtimeRegistry.AddToContext<std::shared_ptr<AnimationSystem>>(
		std::make_shared<AnimationSystem>( runtimeRegistry ) );

	runtimeRegistry.AddToContext<std::shared_ptr<PhysicsSystem>>(
		std::make_shared<PhysicsSystem>( runtimeRegistry ) );

	auto lua = runtimeRegistry.AddToContext<std::shared_ptr<sol::state>>( std::make_shared<sol::state>() );

	if ( !lua )
		lua = std::make_shared<sol::state>();

	lua->open_libraries( sol::lib::base,
						 sol::lib::math,
						 sol::lib::os,
						 sol::lib::table,
						 sol::lib::io,
						 sol::lib::string,
						 sol::lib::package );

	SCION_CORE::Systems::ScriptingSystem::RegisterLuaBindings( *lua, runtimeRegistry );
	SCION_CORE::Systems::ScriptingSystem::RegisterLuaFunctions( *lua, runtimeRegistry );

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
	auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
	auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();
	
	runtimeRegistry.ClearRegistry();
	runtimeRegistry.RemoveContext<std::shared_ptr<Camera2D>>();
	runtimeRegistry.RemoveContext<std::shared_ptr<sol::state>>();
	runtimeRegistry.RemoveContext<std::shared_ptr<SCION_PHYSICS::PhysicsWorld>>();
	runtimeRegistry.RemoveContext<std::shared_ptr<SCION_PHYSICS::ContactListener>>();
	runtimeRegistry.RemoveContext<std::shared_ptr<AnimationSystem>>();
	runtimeRegistry.RemoveContext<std::shared_ptr<PhysicsSystem>>();

	auto& mainRegistry = MAIN_REGISTRY();
	mainRegistry.GetMusicPlayer().Stop();
	mainRegistry.GetSoundPlayer().Stop( -1 );
}

void SceneDisplay::RenderScene()
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& editorFramebuffers = mainRegistry.GetContext<std::shared_ptr<EditorFramebuffers>>();
	auto& renderer = mainRegistry.GetContext<std::shared_ptr<SCION_RENDERING::Renderer>>();

	auto& renderSystem = mainRegistry.GetContext<std::shared_ptr<RenderSystem>>();
	auto& renderUISystem = mainRegistry.GetContext<std::shared_ptr<RenderUISystem>>();
	auto& renderShapeSystem = mainRegistry.GetContext<std::shared_ptr<RenderShapeSystem>>();

	const auto& fb = editorFramebuffers->mapFramebuffers[ FramebufferType::SCENE ];

	fb->Bind();
	renderer->SetViewport( 0, 0, fb->Width(), fb->Height() );
	renderer->SetClearColor( 0.f, 0.f, 0.f, 1.f );
	renderer->ClearBuffers( true, true, false );

	auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
	
	if (pCurrentScene && m_bPlayScene)
	{
		auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();
		renderSystem->Update( runtimeRegistry );
		renderShapeSystem->Update( runtimeRegistry );
		renderUISystem->Update( runtimeRegistry );
	}

	fb->Unbind();
	fb->CheckResize();
}

SceneDisplay::SceneDisplay( )
	: m_bPlayScene{ false }
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

	if ( m_bPlayScene )
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

	RenderScene();

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
		auto& editorFramebuffers = mainRegistry.GetContext<std::shared_ptr<EditorFramebuffers>>();
		const auto& fb = editorFramebuffers->mapFramebuffers[ FramebufferType::SCENE ];

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

	auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
	if ( !pCurrentScene )
		return;
	
	auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();

	auto& mainRegistry = MAIN_REGISTRY();
	auto& coreGlobals = CORE_GLOBALS();

	auto& camera = runtimeRegistry.GetContext<std::shared_ptr<SCION_RENDERING::Camera2D>>();
	if ( !camera )
	{
		SCION_ERROR( "Failed to get the camera from the registry context!" );
		return;
	}

	camera->Update();

	auto& scriptSystem = runtimeRegistry.GetContext<std::shared_ptr<SCION_CORE::Systems::ScriptingSystem>>();
	scriptSystem->Update();

	if ( coreGlobals.IsPhysicsEnabled() )
	{
		auto& pPhysicsWorld = runtimeRegistry.GetContext<SCION_PHYSICS::PhysicsWorld>();
		pPhysicsWorld->Step( one_over_sixty, coreGlobals.GetVelocityIterations(), coreGlobals.GetPositionIterations() );
		pPhysicsWorld->ClearForces();
	}

	auto& pPhysicsSystem = runtimeRegistry.GetContext<std::shared_ptr<SCION_CORE::Systems::PhysicsSystem>>();
	pPhysicsSystem->Update( runtimeRegistry.GetRegistry() );

	auto& animationSystem = runtimeRegistry.GetContext<std::shared_ptr<SCION_CORE::Systems::AnimationSystem>>();
	animationSystem->Update();
}
} // namespace SCION_EDITOR
