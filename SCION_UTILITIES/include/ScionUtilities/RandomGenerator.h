#pragma once
#include <random>

namespace SCION_UTIL
{
class RandomIntGenerator
{
  private:
	std::mt19937 m_RandomEngine;
	std::uniform_int_distribution<std::mt19937::result_type> m_IntDistribution;

	void Initialize();

  public:
	RandomIntGenerator();
	RandomIntGenerator( uint32_t min, uint32_t max );

	int GetValue();
};

class RandomFloatGenerator
{
  private:
	std::mt19937 m_RandomEngine;
	std::uniform_real_distribution<> m_RealDistribution;
	void Initialize();

  public:
	RandomFloatGenerator();
	RandomFloatGenerator( float min, float max );

	float GetValue();
};

} // namespace SCION_UTIL
