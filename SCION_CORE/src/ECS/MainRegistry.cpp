#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include <Logger/Logger.h>
#include <Sounds/MusicPlayer/MusicPlayer.h>
#include <Sounds/SoundPlayer/SoundFxPlayer.h>

namespace SCION_CORE::ECS
{

MainRegistry& MainRegistry::GetInstance()
{
	static MainRegistry instance{};
	return instance;
}

void MainRegistry::Initialize()
{
	m_pMainRegistry = std::make_unique<Registry>();
	SCION_ASSERT( m_pMainRegistry && "Failed to initialize main registry." );

	auto pAssetManager = std::make_shared<SCION_RESOURCES::AssetManager>();
	m_pMainRegistry->AddToContext<std::shared_ptr<SCION_RESOURCES::AssetManager>>( std::move( pAssetManager ) );

	auto pMusicPlayer = std::make_shared<SCION_SOUNDS::MusicPlayer>();
	m_pMainRegistry->AddToContext<std::shared_ptr<SCION_SOUNDS::MusicPlayer>>( std::move( pMusicPlayer ) );

	auto pSoundPlayer = std::make_shared<SCION_SOUNDS::SoundFxPlayer>();
	m_pMainRegistry->AddToContext<std::shared_ptr<SCION_SOUNDS::SoundFxPlayer>>( std::move( pSoundPlayer ) );

	m_bInitialized = true;
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

} // namespace SCION_CORE::ECS