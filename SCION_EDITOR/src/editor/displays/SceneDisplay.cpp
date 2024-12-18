#include "SceneDisplay.h"
#include "Rendering/Buffers/Framebuffer.h"
#include "Rendering/Core/Camera2D.h"
#include "Rendering/Core/Renderer.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/ECS/Components/AllComponents.h"
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
#include "editor/utilities/ImGuiUtils.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"

#include <imgui.h>

using namespace SCION_CORE::Systems;
using namespace SCION_CORE::ECS;
using namespace SCION_RENDERING;

constexpr float one_over_sixty = 1.f / 60.f;

namespace SCION_EDITOR
{
void SceneDisplay::LoadScene()
{
	auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
	if ( !pCurrentScene )
		return;

	pCurrentScene->CopySceneToRuntime();
	auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();

	const auto& canvas = pCurrentScene->GetCanvas();
	runtimeRegistry.AddToContext<std::shared_ptr<Camera2D>>(
		std::make_shared<Camera2D>( canvas.width, canvas.height ) );

	auto pPhysicsWorld = runtimeRegistry.AddToContext<SCION_PHYSICS::PhysicsWorld>(
		std::make_shared<b2World>( b2Vec2{ 0.f, CORE_GLOBALS().GetGravity() } ) );

	auto pContactListener = runtimeRegistry.AddToContext<std::shared_ptr<SCION_PHYSICS::ContactListener>>(
		std::make_shared<SCION_PHYSICS::ContactListener>() );

	pPhysicsWorld->SetContactListener( pContactListener.get() );

	// Add necessary systems
	auto scriptSystem =
		runtimeRegistry.AddToContext<std::shared_ptr<ScriptingSystem>>( std::make_shared<ScriptingSystem>() );

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

	// We need to initialize all of the physics entities
	auto physicsEntities = runtimeRegistry.GetRegistry().view<PhysicsComponent>();

	for ( auto entity : physicsEntities )
	{
		Entity ent{ runtimeRegistry, entity };

		bool bBoxCollider{ ent.HasComponent<BoxColliderComponent>() };
		bool bCircleCollider{ ent.HasComponent<CircleColliderComponent>() };

		if ( !bBoxCollider && !bCircleCollider )
		{
			SCION_ERROR( "Entity must have a box or circle collider component to initialize physics on it." );
			continue;
		}

		auto& physics = ent.GetComponent<PhysicsComponent>();
		auto& physicsAttributes = physics.GetChangableAttributes();

		if ( bBoxCollider )
		{
			const auto& boxCollider = ent.GetComponent<BoxColliderComponent>();
			physicsAttributes.boxSize = glm::vec2{ boxCollider.width, boxCollider.height };
			physicsAttributes.offset = boxCollider.offset;
		}

		else if ( bCircleCollider )
		{
			const auto& circleCollider = ent.GetComponent<CircleColliderComponent>();
			physicsAttributes.radius = circleCollider.radius;
			physicsAttributes.offset = circleCollider.offset;
		}

		const auto& transform = ent.GetComponent<TransformComponent>();
		physicsAttributes.position = transform.position;
		physicsAttributes.scale = transform.scale;
		physicsAttributes.objectData.entityID = static_cast<std::int32_t>( entity );

		/*
		 * TODO: Set Filters/Masks/Group Index
		 */

		physics.Init( pPhysicsWorld, 640, 480 );
	}

	if ( !scriptSystem->LoadMainScript( runtimeRegistry, *lua ) )
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
	runtimeRegistry.RemoveContext<SCION_PHYSICS::PhysicsWorld>();
	runtimeRegistry.RemoveContext<std::shared_ptr<SCION_PHYSICS::ContactListener>>();
	runtimeRegistry.RemoveContext<std::shared_ptr<ScriptingSystem>>();

	auto& mainRegistry = MAIN_REGISTRY();
	mainRegistry.GetMusicPlayer().Stop();
	mainRegistry.GetSoundPlayer().Stop( -1 );
}

void SceneDisplay::RenderScene() const
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& editorFramebuffers = mainRegistry.GetContext<std::shared_ptr<EditorFramebuffers>>();
	auto& renderer = mainRegistry.GetContext<std::shared_ptr<SCION_RENDERING::Renderer>>();

	auto& renderSystem = mainRegistry.GetRenderSystem();
	auto& renderUISystem = mainRegistry.GetRenderUISystem();
	auto& renderShapeSystem = mainRegistry.GetRenderShapeSystem();

	const auto& fb = editorFramebuffers->mapFramebuffers[ FramebufferType::SCENE ];

	fb->Bind();
	renderer->SetViewport( 0, 0, fb->Width(), fb->Height() );
	renderer->SetClearColor( 0.f, 0.f, 0.f, 1.f );
	renderer->ClearBuffers( true, true, false );

	auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();

	if ( pCurrentScene && m_bPlayScene )
	{
		auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();
		auto& camera = runtimeRegistry.GetContext<std::shared_ptr<Camera2D>>();
		renderSystem.Update( runtimeRegistry, *camera );

		if ( CORE_GLOBALS().RenderCollidersEnabled() )
		{
			renderShapeSystem.Update( runtimeRegistry, *camera );
		}

		renderUISystem.Update( runtimeRegistry );
	}

	fb->Unbind();
	fb->CheckResize();
}

void SceneDisplay::DrawToolbar()
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	auto pPlayTexture = assetManager.GetTexture( "play_button" );
	auto pStopTexture = assetManager.GetTexture( "stop_button" );

	SCION_ASSERT( pPlayTexture && pStopTexture );

	ImGui::Separator();
	ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, { 0.f, 0.f } );

	auto playTextureID = (ImTextureID)(intptr_t)pPlayTexture->GetID();
	if ( m_bPlayScene && m_bSceneLoaded )
	{
		ImGui::ActiveImageButton( playTextureID );
	}
	else
	{
		if ( ImGui::ImageButton( playTextureID, TOOL_BUTTON_SIZE ) )
		{
			LoadScene();
		}
	}

	if ( ImGui::IsItemHovered( ImGuiHoveredFlags_DelayNormal ) )
		ImGui::SetTooltip( "Play Scene" );

	ImGui::SameLine();

	auto stopTextureID = (ImTextureID)(intptr_t)pStopTexture->GetID();
	if ( !m_bPlayScene && !m_bSceneLoaded )
	{
		ImGui::ActiveImageButton( stopTextureID );
	}
	else
	{
		if ( ImGui::ImageButton( stopTextureID, TOOL_BUTTON_SIZE ) )
		{
			UnloadScene();
		}
	}

	if ( ImGui::IsItemHovered( ImGuiHoveredFlags_DelayNormal ) )
		ImGui::SetTooltip( "Stop Scene" );

	ImGui::Separator();
	ImGui::PopStyleVar( 1 );
}

SceneDisplay::SceneDisplay()
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

	DrawToolbar();
	RenderScene();

	if ( ImGui::BeginChild(
			 "##SceneChild", ImVec2{ 0.f, 0.f }, ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollWithMouse ) )
	{
		auto& editorFramebuffers = MAIN_REGISTRY().GetContext<std::shared_ptr<EditorFramebuffers>>();
		const auto& fb = editorFramebuffers->mapFramebuffers[ FramebufferType::SCENE ];

		ImGui::SetCursorPos( ImVec2{ 0.f, 0.f } );

		ImGui::Image( (ImTextureID)(intptr_t)fb->GetTextureID(),
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
	scriptSystem->Update( runtimeRegistry );

	if ( coreGlobals.IsPhysicsEnabled() )
	{
		auto& pPhysicsWorld = runtimeRegistry.GetContext<SCION_PHYSICS::PhysicsWorld>();
		pPhysicsWorld->Step( one_over_sixty, coreGlobals.GetVelocityIterations(), coreGlobals.GetPositionIterations() );
		pPhysicsWorld->ClearForces();
	}

	auto& pPhysicsSystem = mainRegistry.GetPhysicsSystem();
	pPhysicsSystem.Update( runtimeRegistry );

	auto& animationSystem = mainRegistry.GetAnimationSystem();
	animationSystem.Update( runtimeRegistry, *camera );
}
} // namespace SCION_EDITOR
