#pragma once

namespace SCION_SOUNDS {
	class SoundFxPlayer
	{
	public:
		SoundFxPlayer() = default;
		~SoundFxPlayer() = default;

		void Play(class SoundFX& soundFx);
		void Play(class SoundFX& soundFx, int loops, int channel);
		void SetVolume(int channel, int volume);
		void Stop(int channel);
		bool IsPlaying(int channel);
	};
}