#include "ScionUtilities/RandomGenerator.h"

namespace SCION_UTIL
{

void RandomIntGenerator::Initialize()
{
	m_RandomEngine.seed( std::random_device()() );
}

RandomIntGenerator::RandomIntGenerator()
	: RandomIntGenerator( 0, std::numeric_limits<uint32_t>::max() )
{
}

RandomIntGenerator::RandomIntGenerator( uint32_t min, uint32_t max )
	: m_RandomEngine{}
	, m_IntDistribution{ min, max }
{
	Initialize();
}

int RandomIntGenerator::GetValue()
{
	return static_cast<int>(m_IntDistribution( m_RandomEngine ));
}

void RandomFloatGenerator::Initialize()
{
	m_RandomEngine.seed( std::random_device()() );
}

RandomFloatGenerator::RandomFloatGenerator()
	: RandomFloatGenerator( 0.f, std::numeric_limits<float>::max() )
{
}

RandomFloatGenerator::RandomFloatGenerator( float min, float max )
	: m_RandomEngine{}
	, m_RealDistribution{ min, max }
{
	Initialize();
}

float RandomFloatGenerator::GetValue()
{
	return m_RealDistribution( m_RandomEngine );
}

} // namespace SCION_UTIL
