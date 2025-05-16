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

constexpr const char* S2D_PRJ_FILE_EXT = ".s2dprj";

namespace SCION_UTIL
{

struct SpriteLayerParams
{
	std::string sLayerName{ "NewLayer" };
	bool bVisible{ true };
};

} // namespace SCION_UTIL
