#pragma once

namespace Scion::Utilities
{
class RandomIntGenerator
{
  public:
	RandomIntGenerator();
	RandomIntGenerator( uint32_t min, uint32_t max );

	int GetValue();

  private:
	std::mt19937 m_RandomEngine;
	std::uniform_int_distribution<std::mt19937::result_type> m_IntDistribution;

	void Initialize();
};

class RandomFloatGenerator
{
  public:
	RandomFloatGenerator();
	RandomFloatGenerator( float min, float max );

	float GetValue();

  private:
	std::mt19937 m_RandomEngine;
	std::uniform_real_distribution<> m_RealDistribution;
	void Initialize();
};

} // namespace Scion::Utilities
