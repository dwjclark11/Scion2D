#include "Sounds/Essentials/SoundFX.h"

Scion::Sounds::SoundFX::SoundFX( const SoundParams& params, SoundFxPtr pSoundFx )
	: m_Params{ params }
	, m_pSoundFx{ std::move( pSoundFx ) }
{
}
