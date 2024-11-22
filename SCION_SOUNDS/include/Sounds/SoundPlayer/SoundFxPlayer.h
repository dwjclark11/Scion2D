#pragma once

namespace SCION_SOUNDS
{
class SoundFxPlayer
{
  public:
	SoundFxPlayer() = default;
	~SoundFxPlayer() = default;

	/*
	 * @brief Calls Mix_PlayChannel and tries to play the desired soundFx.
	 * Assumes not looping and to play on any open channel.
	 * @param A reference to the SoundFx to play.
	 */
	void Play( class SoundFX& soundFx );

	/*
	 * @brief Calls Mix_PlayChannel and tries to play the desired soundFx
	 * @param A reference to the SoundFx to play.
	 * @param An int for the number of times the soundFx should loop.
	 * @param An int for the channel to play the music on. Pass in -1 to play on any open channel.
	 */
	void Play( class SoundFX& soundFx, int loops, int channel = -1 );

	/*
	 * @brief Sets the volume for the specified sound channel
	 * @param Takes in an int for the channel we want to set the volume.
	 * [-1] Sets the volume for all channels.
	 * @param takes in an int value for the volume. The value needs
	 * to be between [0-100] | 0 == MIN | 100 == MAX
	 */
	void SetVolume( int volume, int channel = -1 );

	/*
	 * @brief Halts the sound fx for the specified channel..
	 * @param an int for the desired channel to stop playing.
	 */
	void Stop( int channel );

	/*
	 * @brief Returns true if the specified channel is currently playing
	 * @param an int for the desired channel to check.
	 */
	bool IsPlaying( int channel );
};
} // namespace SCION_SOUNDS
