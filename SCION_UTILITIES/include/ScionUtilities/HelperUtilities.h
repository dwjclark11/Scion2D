#pragma once
#include <string>

#define BASE_PATH                                                                                                      \
	std::string                                                                                                        \
	{                                                                                                                  \
		SDL_GetBasePath()                                                                                              \
	}

#ifdef _WIN32
constexpr char PATH_SEPARATOR = '\\';
#define DEFAULT_PROJECT_PATH BASE_PATH + "ScionProjects"
#else
constexpr char PATH_SEPARATOR = '/';
#define DEFAULT_PROJECT_PATH BASE_PATH + PATH_SEPARATOR + "ScionProjects"
#endif

#define SCRIPTS "scripts"
#define ASSETS "assets"
#define CONTENT "content"

constexpr const std::string_view CONTENT_FOLDER = "content";
constexpr const std::string_view S2D_PRJ_FILE_EXT = ".s2dprj";

namespace SCION_UTIL
{

struct SpriteLayerParams
{
	std::string sLayerName{ "NewLayer" };
	bool bVisible{ true };
	int layer{ -1 };

	friend bool operator==(const SpriteLayerParams& a, const SpriteLayerParams& b)
	{
		return a.sLayerName == b.sLayerName && a.bVisible == b.bVisible && a.layer == b.layer;
	}
};

} // namespace SCION_UTIL
