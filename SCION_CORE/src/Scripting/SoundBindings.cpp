#include "Core/Scripting/SoundBindings.h"
#include "Core/ECS/Registry.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include <Sounds/AudioPlayer/AudioPlayer.hpp>
#include <Sounds/Essentials/Audio.hpp>

#include <Logger/Logger.h>

using namespace Scion::Sounds;
using namespace Scion::Resources;

void Scion::Core::Scripting::SoundBinder::CreateSoundBind( sol::state& lua )
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& audioPlayer = mainRegistry.GetAudioPlayer();
	auto& assetManager = mainRegistry.GetAssetManager();

	// clang-format off
	lua.new_usertype<AudioPlayer>(
		"AudioPlayer",
		sol::no_constructor,
		"get", [&audioPlayer](sol::this_state s) { return sol::make_reference(s, std::ref(audioPlayer)); },
		"playTrack", sol::overload(
			[&audioPlayer](int trackNum) {
				return audioPlayer.PlayTrack( trackNum );
			},
			[&audioPlayer](int trackNum, int loops) {
				return audioPlayer.PlayTrack( trackNum, loops);
			},
			[&audioPlayer, &assetManager](int trackNum, const std::string& audioName) {
				auto* pAudio = assetManager.GetAudio( audioName );
				if (!pAudio || !pAudio->GetAudioPtr())
				{
					SCION_ERROR("Failed to play track. Audio [{}] does not exist in asset manager or is invalid.", audioName);
					return false;
				}

				return audioPlayer.PlayTrack( trackNum, pAudio->GetAudioPtr(), 0);
			},
			[&audioPlayer, &assetManager](int trackNum, const std::string& audioName, int loops) {
				auto* pAudio = assetManager.GetAudio( audioName );
				if (!pAudio || !pAudio->GetAudioPtr())
				{
					SCION_ERROR("Failed to play track. Audio [{}] does not exist in asset manager or is invalid.", audioName);
					return false;
				}

				return audioPlayer.PlayTrack( trackNum, pAudio->GetAudioPtr(), loops);
			}
		),
		"stopTrack", sol::overload(
			[&audioPlayer] (int trackNum ) { return audioPlayer.StopTrack(trackNum, 0); },
			[&audioPlayer] (int trackNum, int fadeOutFrames ) { return audioPlayer.StopTrack(trackNum, fadeOutFrames); }
		),
		"stopAllTracks", [&audioPlayer] { audioPlayer.StopAllTracks(); },
		"pauseTrack", [&audioPlayer] (int trackNum) { return audioPlayer.PauseTrack(trackNum); },
		"resumeTrack", [&audioPlayer] (int trackNum) { return audioPlayer.ResumeTrack(trackNum); },
		"fadeInAudio",[&audioPlayer, &assetManager] (int trackNum, const std::string& audioName, int milliseconds)
		{
			auto* pAudio = assetManager.GetAudio( audioName );
				if (!pAudio || !pAudio->GetAudioPtr())
				{
					SCION_ERROR("Failed to Fade in audio. Audio [{}] does not exist in asset manager or is invalid.", audioName);
					return false;
				}

			return audioPlayer.FadeInAudio(trackNum, pAudio->GetAudioPtr(), milliseconds);
		},
		"setTrackVolume", [&audioPlayer] (int trackNum, float volume) { return audioPlayer.SetTrackVolume(trackNum, volume); },
		"isTrackPlaying", [&audioPlayer] (int trackNum) { return audioPlayer.IsTrackPlaying(trackNum); },
		"isTrackPaused", [&audioPlayer] (int trackNum) { return audioPlayer.IsTrackPaused(trackNum); }
	);
	// clang-format on
}
