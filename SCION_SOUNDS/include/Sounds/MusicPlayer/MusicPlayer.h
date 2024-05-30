#pragma once

namespace SCION_SOUNDS
{
/*
 * @brief The MusicPlayer class is a simple wrapper around SDL_Mixer for music.
 */
class MusicPlayer
{
  public:
	/*
	 * @brief CTOR -- Opens the Mix Audio Device.
	 * @brief Uses 44100 for Frquency, Default Format, 2 channels for Stereo sound, and
	 * 4096 for the chunksize
	 */
	MusicPlayer();

	/*
	 * @brief DTOR -- Stops all music and closes the audio device
	 */
	~MusicPlayer();

	/*
	 * @brief Calls Mix_PlayMusic and tries to play the desired music
	 * @param Takes a Reference to the Music and the number loops to play
	 * the music. Use '-1' to play music indefinitely.
	 */
	void Play(class Music& music, int loops = 0);

	/*
	 * @brief Pauses the current playing music.
	 */
	void Pause();

	/*
	 * @brief Resumes the current paused music.
	 */
	void Resume();

	/*
	 * @brief Halts the music if it is playing.
	 */
	void Stop();

	/*
	 * @brief Sets the volume for the music channel
	 * @param takes in an int value for the volume. The value needs
	 * to be between [0-100] | 0 == MIN | 100 == MAX
	 */
	void SetVolume(int volume);

	/*
	 * @brief Returns true if the music channel is currently playing
	 */
	bool IsPlaying();
};
} // namespace SCION_SOUNDS