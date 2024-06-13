#pragma once
#include <string>

namespace SCION_SOUNDS
{
struct SoundParams
{
	std::string name{ "" }, description{ "" }, filename{ "" };
	double duration{ 0.0 };
};
} // namespace SCION_SOUNDS