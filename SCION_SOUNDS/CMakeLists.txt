add_library(SCION_SOUNDS
    "include/Sounds/Essentials/Music.h"
    "src/Music.cpp"
    "include/Sounds/Essentials/SoundFX.h"
    "src/SoundFX.cpp"
    "include/Sounds/Essentials/SoundParams.h"

    "include/Sounds/MusicPlayer/MusicPlayer.h"
    "src/MusicPlayer.cpp"

    "include/Sounds/SoundPlayer/SoundFxPlayer.h"
    "src/SoundFxPlayer.cpp")

target_include_directories(
    SCION_SOUNDS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(SCION_SOUNDS
    PRIVATE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    PUBLIC SCION_UTILITIES)