#pragma once

namespace SCION_EDITOR
{
constexpr const char* DROP_TEXTURE_SRC = "DropTextureSource";
constexpr const char* DROP_FONT_SRC = "DropFontSource";
constexpr const char* DROP_SOUNDFX_SRC = "DropSoundFxSource";
constexpr const char* DROP_MUSIC_SRC = "DropMusicSource";
constexpr const char* DROP_SCENE_SRC = "DropSceneSource";


struct Canvas
{
	int width{ 640 }, height{ 480 }, tileWidth{ 16 }, tileHeight{ 16 };
};

}