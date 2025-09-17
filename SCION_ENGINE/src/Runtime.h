#pragma once
#include <SDL2/SDL.h>

namespace sol
{
class state;
}

namespace SCION_WINDOWING
{
class Window;
}

namespace SCION_CORE
{
struct GameConfig;
}

namespace SCION_UTIL
{
enum class AssetType;
struct S2DAsset;
} // namespace SCION_UTIL

namespace SCION_ENGINE
{
class RuntimeApp
{
  public:
	RuntimeApp();
	~RuntimeApp();

	void Run();

  private:
	void Initialize();

	bool LoadShaders();
	bool LoadConfig( sol::state& lua );
	bool LoadRegistryContext();
	void LoadBindings();
	bool LoadScripts();
	bool LoadPhysics();
	bool LoadZip();

	void ProcessEvents();
	void Update();
	void Render();

	void CleanUp();

  private:
	std::unique_ptr<SCION_WINDOWING::Window> m_pWindow;
	std::unique_ptr<SCION_CORE::GameConfig> m_pGameConfig;
	std::unordered_map<SCION_UTIL::AssetType, std::vector<std::unique_ptr<SCION_UTIL::S2DAsset>>> m_mapS2DAssets;
	SDL_Event m_Event;
	bool m_bRunning;
	/*
	   Hack to deal with a change in allocated channels.
	   The channels are automatically allocated in the music player in
	   the main registry; however, the game config could have more or less
	   channels to allocate. This needs to be done after the config and registry
	   have already been initialized.
	*/
	int m_DeltaAllocatedChannels;
};
} // namespace SCION_ENGINE
