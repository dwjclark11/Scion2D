#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include <Logger/Logger.h>
#include <Core/Systems/ScriptingSystem.h>
#include <Core/Systems/RenderSystem.h>
#include <Core/Systems/RenderUISystem.h>
#include <Core/Systems/RenderShapeSystem.h>
#include <Core/Systems/AnimationSystem.h>
#include <Core/Systems/PhysicsSystem.h>
#include <Core/Events/EventDispatcher.h>
#include <Rendering/Core/Renderer.h>
#include <ScionUtilities/HelperUtilities.h>

#include <Sounds/AudioPlayer/AudioPlayer.hpp>

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

	AddToContext( std::make_unique<Scion::Resources::AssetManager>( bEnableFilewatcher ) );

	SDL_AudioSpec spec;
	SDL_zero( spec );
	spec.freq = 44100;
	spec.format = SDL_AUDIO_F32;
	spec.channels = 2;

	m_pMainRegistry->AddToContext( std::make_unique<Scion::Sounds::AudioPlayer>( 32, spec ) );
	auto& renderer = AddToContext( std::make_unique<Scion::Rendering::Renderer>() );

	// Enable Alpha Blending
	renderer->SetCapability( Scion::Rendering::Renderer::GLCapability::BLEND, true );
	renderer->SetBlendCapability( Scion::Rendering::Renderer::BlendingFactors::SRC_ALPHA,
								  Scion::Rendering::Renderer::BlendingFactors::ONE_MINUS_SRC_ALPHA );

	m_bInitialized = RegisterMainSystems();

	return m_bInitialized;
}

bool MainRegistry::RegisterMainSystems()
{
	AddToContext( std::make_unique<Scion::Core::Systems::RenderSystem>() );
	AddToContext( std::make_unique<Scion::Core::Systems::RenderUISystem>() );
	AddToContext( std::make_unique<Scion::Core::Systems::RenderShapeSystem>() );
	AddToContext( std::make_unique<Scion::Core::Systems::PhysicsSystem>() );
	AddToContext( std::make_unique<Scion::Core::Systems::AnimationSystem>() );
	AddToContext( std::make_unique<Scion::Core::Events::EventDispatcher>() );

	return true;
}

Scion::Core::Events::EventDispatcher& MainRegistry::GetEventDispatcher()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::unique_ptr<Scion::Core::Events::EventDispatcher>>();
}

Scion::Resources::AssetManager& MainRegistry::GetAssetManager()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::unique_ptr<Scion::Resources::AssetManager>>();
}

Scion::Sounds::AudioPlayer& MainRegistry::GetAudioPlayer()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::unique_ptr<Scion::Sounds::AudioPlayer>>();
}

Scion::Rendering::Renderer& MainRegistry::GetRenderer()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::unique_ptr<Scion::Rendering::Renderer>>();
}

Scion::Core::Systems::RenderSystem& MainRegistry::GetRenderSystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::unique_ptr<Scion::Core::Systems::RenderSystem>>();
}

Scion::Core::Systems::RenderUISystem& MainRegistry::GetRenderUISystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::unique_ptr<Scion::Core::Systems::RenderUISystem>>();
}

Scion::Core::Systems::RenderShapeSystem& MainRegistry::GetRenderShapeSystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::unique_ptr<Scion::Core::Systems::RenderShapeSystem>>();
}

Scion::Core::Systems::AnimationSystem& MainRegistry::GetAnimationSystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::unique_ptr<Scion::Core::Systems::AnimationSystem>>();
}

Scion::Core::Systems::PhysicsSystem& MainRegistry::GetPhysicsSystem()
{
	SCION_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::unique_ptr<Scion::Core::Systems::PhysicsSystem>>();
}

Registry* MainRegistry::GetRegistry()
{
	if ( !m_pMainRegistry )
		m_pMainRegistry = std::make_unique<Registry>();

	return m_pMainRegistry.get();
}

bool MainRegistry::CleanUp()
{
	m_pMainRegistry.reset( nullptr );
	return m_pMainRegistry == nullptr;
}

} // namespace Scion::Core::ECS
