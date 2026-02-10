#pragma once
#include <string>

namespace Scion::Sounds
{
struct SoundParams
{
	std::string name{};
	std::string description{};
	std::string filename{};
	double duration{ 0.0 };
};
} // namespace Scion::Sounds
