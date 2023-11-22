#include "MusicPlayer.h"
#include "../Essentials/Music.h"
#include <Logger.h>

namespace SCION_SOUNDS {

	MusicPlayer::MusicPlayer()
	{
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
		{
			std::string error{Mix_GetError()};
			SCION_ERROR("Unable to open SDL Music Mixer - {}", error);
			return;
		}
		SCION_LOG("CHANNELS ALLOCATED [{}]", Mix_AllocateChannels(16));
	}

	MusicPlayer::~MusicPlayer()
	{
		Mix_HaltMusic();
		Mix_CloseAudio();
		Mix_Quit();
		SCION_LOG("Music Player Closed!");
	}

	void MusicPlayer::Play(Music& music, int loops)
	{
		if (!music.GetMusicPtr())
		{
			SCION_ERROR("Failed to play music [{}] - Mix Music was Null!", music.GetName());
			return;
		}

		if (Mix_PlayMusic(music.GetMusicPtr(), loops) != 0)
		{
			std::string error{Mix_GetError()};
			SCION_ERROR("Failed to play music [{}] Mix Error - {}", music.GetName(), error);
		}
	}

	void MusicPlayer::Pause()
	{

	}

	void MusicPlayer::Resume()
	{

	}

	void MusicPlayer::Stop()
	{

	}

	void MusicPlayer::SetVolume(int volume)
	{

	}

	bool MusicPlayer::IsPlaying()
	{
		return false;
	}

}