#pragma once
#include <memory>
#include "Core/ECS/Registry.h"

#define MAIN_REGISTRY() SCION_CORE::ECS::MainRegistry::GetInstance()

namespace SCION_RESOURCES
{
class AssetManager;
}
namespace SCION_SOUNDS
{
class MusicPlayer;
class SoundFxPlayer;
} // namespace SCION_SOUNDS

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
  private:
	std::unique_ptr<Registry> m_pMainRegistry{ nullptr };
	bool m_bInitialized{ false };

	MainRegistry() = default;
	~MainRegistry() = default;
	MainRegistry( const MainRegistry& ) = delete;
	MainRegistry& operator=( const MainRegistry& ) = delete;

	bool RegisterMainSystems();

  public:
	static MainRegistry& GetInstance();
	bool Initialize();

	SCION_RESOURCES::AssetManager& GetAssetManager();
	SCION_SOUNDS::MusicPlayer& GetMusicPlayer();
	SCION_SOUNDS::SoundFxPlayer& GetSoundPlayer();

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
};
} // namespace SCION_CORE::ECS
