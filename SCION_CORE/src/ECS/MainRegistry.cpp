#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include <Logger/Logger.h>
#include <Sounds/MusicPlayer/MusicPlayer.h>
#include <Sounds/SoundPlayer/SoundFxPlayer.h>
#include <Core/Systems/ScriptingSystem.h>
#include <Core/Systems/RenderSystem.h>
#include <Core/Systems/RenderUISystem.h>
#include <Core/Systems/RenderShapeSystem.h>
#include <Core/Systems/AnimationSystem.h>
#include <Core/Systems/PhysicsSystem.h>
#include <Core/Events/EventDispatcher.h>
#include <Rendering/Core/Renderer.h>
#include <ScionUtilities/HelperUtilities.h>

// Systems not needed outside of the editor
#ifdef IN_SCION_EDITOR
#include "Core/Systems/RenderPickingSystem.h"
#endif
// End Editor only systems

namespace Scion::Core::ECS
{

MainRegistry& MainRegistry::GetInstance()
{
	static MainRegistry instance{};
	return instance;
}

bool MainRegistry::Initialize( bool bEnableFilewatcher )
{
	m_pMainRegistry = std::make_unique<Registry>();
	SCION_ASSERT( m_pMainRegistry && "Failed to initialize main registry." );

	auto pAssetManager = std::make_shared<SCION_RESOURCES::AssetManager>( bEnableFilewatcher );
	m_pMainRegistry->AddToContext<std::shared_ptr<SCION_RESOURCES::AssetManager>>( std::move( pAssetManager ) );

	auto pMusicPlayer = std::make_shared<Scion::Sounds::MusicPlayer>();
	m_pMainRegistry->AddToContext<std::shared_ptr<Scion::Sounds::MusicPlayer>>( std::move( pMusicPlayer ) );

	auto pSoundPlayer = std::make_shared<Scion::Sounds::SoundFxPlayer>();
	m_pMainRegistry->AddToContext<std::shared_ptr<Scion::Sounds::SoundFxPlayer>>( std::move( pSoundPlayer ) );

	auto renderer = std::make_shared<Scion::Rendering::Renderer>();

	// Enable Alpha Blending
	renderer->SetCapability( Scion::Rendering::Renderer::GLCapability::BLEND, true );
	renderer->SetBlendCapability( Scion::Rendering::Renderer::BlendingFactors::SRC_ALPHA,
								  Scion::Rendering::Renderer::BlendingFactors::ONE_MINUS_SRC_ALPHA );

	// Currently we only need the depth test in the editor.
#ifdef IN_SCION_EDITOR
	renderer->SetCapability( Scion::Rendering::Renderer::GLCapability::DEPTH_TEST, true );
#endif

	if ( !AddToContext<std::shared_ptr<Scion::Rendering::Renderer>>( renderer ) )
	{
		SCION_ERROR( "Failed to add the renderer to the registry context!" );
		return false;
	}

	m_bInitialized = RegisterMainSystems();

	return m_bInitialized;
}

bool MainRegistry::RegisterMainSystems()
{
	auto renderSystem = std::make_shared<Scion::Core::Systems::RenderSystem>();
	if ( !renderSystem )
	{
		SCION_ERROR( "Failed to create the render system!" );
		return false;
	}

	if ( !AddToContext<std::shared_ptr<Scion::Core::Systems::RenderSystem>>( renderSystem ) )
	{
		SCION_ERROR( "Failed to add the render system to the registry context!" );
		return false;
	}

	auto renderUISystem = std::make_shared<Scion::Core::Systems::RenderUISystem>();
	if ( !renderUISystem )
	{
		SCION_ERROR( "Failed to create the render UI system!" );
		return false;
	}

	if ( !AddToContext<std::shared_ptr<Scion::Core::Systems::RenderUISystem>>( renderUISystem ) )
	{
		SCION_ERROR( "Failed to add the render UI system to the registry context!" );
		return false;
	}

	auto renderShapeSystem = std::make_shared<Scion::Core::Systems::RenderShapeSystem>();
	if ( !renderShapeSystem )
	{
		SCION_ERROR( "Failed to create the render Shape system!" );
		return false;
	}

	if ( !AddToContext<std::shared_ptr<Scion::Core::Systems::RenderShapeSystem>>( renderShapeSystem ) )
	{
		SCION_ERROR( "Failed to add the render Shape system to the registry context!" );
		return false;
	}

	AddToContext<std::shared_ptr<Scion::Core::Systems::PhysicsSystem>>(
		std::make_shared<Scion::Core::Systems::PhysicsSystem>() );

	AddToContext<std::shared_ptr<Scion::Core::Systems::AnimationSystem>>(
		std::make_shared<Scion::Core::Systems::AnimationSystem>() );

	AddToContext<std::shared_ptr<Scion::Core::Events::EventDispatcher>>(
		std::make_shared<Scion::Core::Events::EventDispatcher>() );

#ifdef IN_SCION_EDITOR
	AddToContext<std::shared_ptr<Scion::Core::Systems::RenderPickingSystem>>(
		std::make_shared<Scion::Core::Systems::RenderPickingSystem>() );
	SCION_LOG( "Added Render Picking System to Main Registry." );
#endif

	return true;
}

Scion::Core::Events::EventDispatcher& MainRegistry::GetEventDispatcher()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Scion::Core::Events::EventDispatcher>>();
}

SCION_RESOURCES::AssetManager& MainRegistry::GetAssetManager()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<SCION_RESOURCES::AssetManager>>();
}

Scion::Sounds::MusicPlayer& MainRegistry::GetMusicPlayer()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Scion::Sounds::MusicPlayer>>();
}

Scion::Sounds::SoundFxPlayer& MainRegistry::GetSoundPlayer()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Scion::Sounds::SoundFxPlayer>>();
}

Scion::Rendering::Renderer& MainRegistry::GetRenderer()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Scion::Rendering::Renderer>>();
}

Scion::Core::Systems::RenderSystem& MainRegistry::GetRenderSystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Scion::Core::Systems::RenderSystem>>();
}

Scion::Core::Systems::RenderUISystem& MainRegistry::GetRenderUISystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Scion::Core::Systems::RenderUISystem>>();
}

Scion::Core::Systems::RenderShapeSystem& MainRegistry::GetRenderShapeSystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Scion::Core::Systems::RenderShapeSystem>>();
}

Scion::Core::Systems::AnimationSystem& MainRegistry::GetAnimationSystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Scion::Core::Systems::AnimationSystem>>();
}

Scion::Core::Systems::PhysicsSystem& MainRegistry::GetPhysicsSystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Scion::Core::Systems::PhysicsSystem>>();
}

Registry* MainRegistry::GetRegistry()
{
	if ( !m_pMainRegistry )
		m_pMainRegistry = std::make_unique<Registry>();

	return m_pMainRegistry.get();
}


} // namespace Scion::Core::ECS
