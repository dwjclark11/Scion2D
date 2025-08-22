#pragma once
#include "Core/ECS/Registry.h"

#define MAIN_REGISTRY() SCION_CORE::ECS::MainRegistry::GetInstance()
#define ASSET_MANAGER() MAIN_REGISTRY().GetAssetManager()
#define EVENT_DISPATCHER() MAIN_REGISTRY().GetEventDispatcher()
#define ADD_EVENT_HANDLER( Event, Func, Handler ) EVENT_DISPATCHER().AddHandler<Event, Func>( Handler );

namespace SCION_RESOURCES
{
class AssetManager;
}
namespace SCION_SOUNDS
{
class MusicPlayer;
class SoundFxPlayer;
} // namespace SCION_SOUNDS

namespace SCION_CORE::Events
{
class EventDispatcher;
}

namespace SCION_RENDERING
{
class Renderer;
}

namespace SCION_CORE::Systems
{
class RenderSystem;
class RenderUISystem;
class RenderShapeSystem;
class AnimationSystem;
class PhysicsSystem;
} // namespace SCION_CORE::Systems

namespace SCION_CORE::ECS
{
class MainRegistry
{
  public:
	static MainRegistry& GetInstance();
	bool Initialize( bool bEnableFilewatcher = false );

	SCION_CORE::Events::EventDispatcher& GetEventDispatcher();
	SCION_RESOURCES::AssetManager& GetAssetManager();
	SCION_SOUNDS::MusicPlayer& GetMusicPlayer();
	SCION_SOUNDS::SoundFxPlayer& GetSoundPlayer();
	SCION_RENDERING::Renderer& GetRenderer();

	template <typename TContext>
	TContext AddToContext( TContext context )
	{
		return m_pMainRegistry->AddToContext<TContext>( context );
	}

	template <typename TContext>
	TContext& GetContext()
	{
		return m_pMainRegistry->GetContext<TContext>();
	}

	SCION_CORE::Systems::RenderSystem& GetRenderSystem();
	SCION_CORE::Systems::RenderUISystem& GetRenderUISystem();
	SCION_CORE::Systems::RenderShapeSystem& GetRenderShapeSystem();
	SCION_CORE::Systems::AnimationSystem& GetAnimationSystem();
	SCION_CORE::Systems::PhysicsSystem& GetPhysicsSystem();
	Registry* GetRegistry();

  private:
	MainRegistry() = default;
	~MainRegistry() = default;
	MainRegistry( const MainRegistry& ) = delete;
	MainRegistry& operator=( const MainRegistry& ) = delete;

	bool RegisterMainSystems();

  private:
	std::unique_ptr<Registry> m_pMainRegistry{ nullptr };
	bool m_bInitialized{ false };
};
} // namespace SCION_CORE::ECS
