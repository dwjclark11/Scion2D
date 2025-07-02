#pragma once
#include <string>

namespace SCION_CORE
{
struct SaveProject
{
	/* The name of the project. */
	std::string sProjectName{};
	/* The active path of the project. */
	std::string sProjectPath{};
	/* The active project file path. */
	std::string sProjectFilePath{};
	/* The main lua script path of the project. */
	std::string sMainLuaScript{};
	/* Game exe file icon path. */
	std::string sFileIconPath{};
	/* The path to the script_list.lua file. */
	std::string sScriptListPath{};
};

struct GameConfig
{
	std::string sGameName{};
	std::string sStartupScene{};

	int windowWidth{ 640 };
	int windowHeight{ 480 };
	uint32_t windowFlags{ 0 };

	int cameraWidth{ 640 };
	int cameraHeight{ 480 };
	float cameraScale{ 1.f };

	bool bPhysicsEnabled{ false };
	int32_t positionIterations{ 8 };
	int32_t velocityIterations{ 8 };
	float gravity{ 9.8f };

	bool bPackageAssets{ false };

	void Reset()
	{
		sGameName.clear();
		sStartupScene.clear();

		windowWidth = 640;
		windowHeight = 480;
		windowFlags = 0;

		cameraWidth = 640;
		cameraHeight = 480;
		cameraScale = 1.f;

		bPhysicsEnabled = false;
		positionIterations = 8;
		velocityIterations = 8;
		gravity = 9.8f;

		bPackageAssets = false;
	}
};

} // namespace SCION_CORE
