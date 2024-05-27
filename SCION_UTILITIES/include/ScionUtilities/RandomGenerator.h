#pragma once
#include <random>

namespace SCION_UTIL {
	class RandomGenerator
	{
	private:
		std::mt19937 m_RandomEngine;
		std::uniform_int_distribution<std::mt19937::result_type> m_Distribution;
		void Initialize();

	public:
		RandomGenerator();
		RandomGenerator(uint32_t min, uint32_t max);

		float GetFloat();
		uint32_t GetInt();
	};
}