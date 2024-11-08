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
#include <Rendering/Core/Renderer.h>
#include <ScionUtilities/HelperUtilities.h>

namespace SCION_CORE::ECS
{

MainRegistry& MainRegistry::GetInstance()
{
	static MainRegistry instance{};
	return instance;
}

bool MainRegistry::Initialize()
{
	m_pMainRegistry = std::make_unique<Registry>();
	SCION_ASSERT( m_pMainRegistry && "Failed to initialize main registry." );

	auto pAssetManager = std::make_shared<SCION_RESOURCES::AssetManager>();
	m_pMainRegistry->AddToContext<std::shared_ptr<SCION_RESOURCES::AssetManager>>( std::move( pAssetManager ) );

	auto pMusicPlayer = std::make_shared<SCION_SOUNDS::MusicPlayer>();
	m_pMainRegistry->AddToContext<std::shared_ptr<SCION_SOUNDS::MusicPlayer>>( std::move( pMusicPlayer ) );

	auto pSoundPlayer = std::make_shared<SCION_SOUNDS::SoundFxPlayer>();
	m_pMainRegistry->AddToContext<std::shared_ptr<SCION_SOUNDS::SoundFxPlayer>>( std::move( pSoundPlayer ) );

	auto renderer = std::make_shared<SCION_RENDERING::Renderer>();

	// Enable Alpha Blending
	renderer->SetCapability( SCION_RENDERING::Renderer::GLCapability::BLEND, true );
	renderer->SetBlendCapability( SCION_RENDERING::Renderer::BlendingFactors::SRC_ALPHA,
								  SCION_RENDERING::Renderer::BlendingFactors::ONE_MINUS_SRC_ALPHA );

	renderer->SetLineWidth( 4.f );

	if ( !AddToContext<std::shared_ptr<SCION_RENDERING::Renderer>>( renderer ) )
	{
		SCION_ERROR( "Failed to add the renderer to the registry context!" );
		return false;
	}

	m_bInitialized = RegisterMainSystems();

	return m_bInitialized;
}

bool MainRegistry::RegisterMainSystems()
{
	auto renderSystem = std::make_shared<SCION_CORE::Systems::RenderSystem>();
	if ( !renderSystem )
	{
		SCION_ERROR( "Failed to create the render system!" );
		return false;
	}

	if ( !AddToContext<std::shared_ptr<SCION_CORE::Systems::RenderSystem>>( renderSystem ) )
	{
		SCION_ERROR( "Failed to add the render system to the registry context!" );
		return false;
	}

	auto renderUISystem = std::make_shared<SCION_CORE::Systems::RenderUISystem>();
	if ( !renderUISystem )
	{
		SCION_ERROR( "Failed to create the render UI system!" );
		return false;
	}

	if ( !AddToContext<std::shared_ptr<SCION_CORE::Systems::RenderUISystem>>( renderUISystem ) )
	{
		SCION_ERROR( "Failed to add the render UI system to the registry context!" );
		return false;
	}

	auto renderShapeSystem = std::make_shared<SCION_CORE::Systems::RenderShapeSystem>();
	if ( !renderShapeSystem )
	{
		SCION_ERROR( "Failed to create the render Shape system!" );
		return false;
	}

	if ( !AddToContext<std::shared_ptr<SCION_CORE::Systems::RenderShapeSystem>>( renderShapeSystem ) )
	{
		SCION_ERROR( "Failed to add the render Shape system to the registry context!" );
		return false;
	}

	AddToContext<std::shared_ptr<SCION_CORE::Systems::PhysicsSystem>>(
		std::make_shared<SCION_CORE::Systems::PhysicsSystem>() );

	AddToContext<std::shared_ptr<SCION_CORE::Systems::AnimationSystem>>(
		std::make_shared<SCION_CORE::Systems::AnimationSystem>() );

	return true;
}

SCION_RESOURCES::AssetManager& MainRegistry::GetAssetManager()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<SCION_RESOURCES::AssetManager>>();
}

SCION_SOUNDS::MusicPlayer& MainRegistry::GetMusicPlayer()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<SCION_SOUNDS::MusicPlayer>>();
}

SCION_SOUNDS::SoundFxPlayer& MainRegistry::GetSoundPlayer()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<SCION_SOUNDS::SoundFxPlayer>>();
}

SCION_CORE::Systems::RenderSystem& MainRegistry::GetRenderSystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<SCION_CORE::Systems::RenderSystem>>();
}

SCION_CORE::Systems::RenderUISystem& MainRegistry::GetRenderUISystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<SCION_CORE::Systems::RenderUISystem>>();
}

SCION_CORE::Systems::RenderShapeSystem& MainRegistry::GetRenderShapeSystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<SCION_CORE::Systems::RenderShapeSystem>>();
}

SCION_CORE::Systems::AnimationSystem& MainRegistry::GetAnimationSystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<SCION_CORE::Systems::AnimationSystem>>();
}

SCION_CORE::Systems::PhysicsSystem& MainRegistry::GetPhysicsSystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<SCION_CORE::Systems::PhysicsSystem>>();
}

} // namespace SCION_CORE::ECS
