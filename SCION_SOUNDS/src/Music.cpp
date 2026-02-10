#include "Sounds/Essentials/Music.h"

Scion::Sounds::Music::Music( const SoundParams& params, MusicPtr pMusic )
	: m_Params{ params }
	, m_pMusic{ std::move( pMusic ) }
{
}
