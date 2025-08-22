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
};
} // namespace SCION_ENGINE
