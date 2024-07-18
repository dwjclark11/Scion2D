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

  public:
	static MainRegistry& GetInstance();
	void Initialize();

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
};
} // namespace SCION_CORE::ECS