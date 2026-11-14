#pragma once
#include "Core/ECS/Registry.h"
#include <utility>

#define MAIN_REGISTRY() Scion::Core::ECS::MainRegistry::GetInstance()
#define ASSET_MANAGER() MAIN_REGISTRY().GetAssetManager()
#define EVENT_DISPATCHER() MAIN_REGISTRY().GetEventDispatcher()
#define ADD_EVENT_HANDLER( Event, Func, Handler ) EVENT_DISPATCHER().AddHandler<Event, Func>( Handler );

namespace Scion::Resources
{
class AssetManager;
}
namespace Scion::Sounds
{
class AudioPlayer;
} // namespace Scion::Sounds

namespace Scion::Core::Events
{
class EventDispatcher;
}

namespace Scion::Rendering
{
class Renderer;
}

namespace Scion::Core::Systems
{
class RenderSystem;
class RenderUISystem;
class RenderShapeSystem;
class AnimationSystem;
class PhysicsSystem;
} // namespace Scion::Core::Systems

namespace Scion::Core::ECS
{
class MainRegistry
{
  public:
	static MainRegistry& GetInstance();
	bool Initialize( bool bEnableFilewatcher = false );

	Scion::Core::Events::EventDispatcher& GetEventDispatcher();
	Scion::Resources::AssetManager& GetAssetManager();
	Scion::Sounds::AudioPlayer& GetAudioPlayer();
	Scion::Rendering::Renderer& GetRenderer();

	template <typename TContext>
	std::remove_cvref_t<TContext>& AddToContext( TContext&& context )
	{
		using Type = std::remove_cvref_t<TContext>;
		return m_pMainRegistry->AddToContext<Type>( std::forward<TContext>( context ) );
	}

	template <typename TContext>
	TContext& GetContext()
	{
		return m_pMainRegistry->GetContext<TContext>();
	}

	Scion::Core::Systems::RenderSystem& GetRenderSystem();
	Scion::Core::Systems::RenderUISystem& GetRenderUISystem();
	Scion::Core::Systems::RenderShapeSystem& GetRenderShapeSystem();
	Scion::Core::Systems::AnimationSystem& GetAnimationSystem();
	Scion::Core::Systems::PhysicsSystem& GetPhysicsSystem();
	Registry* GetRegistry();

	bool CleanUp();

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
} // namespace Scion::Core::ECS
