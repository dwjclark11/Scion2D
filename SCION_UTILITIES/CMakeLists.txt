add_library(SCION_UTILITIES
    "include/ScionUtilities/RandomGenerator.h"
    "src/RandomGenerator.cpp"
    "include/ScionUtilities/SDL_Wrappers.h"
    "src/SDL_Wrappers.cpp"
    "include/ScionUtilities/Timer.h"
    "src/Timer.cpp")

target_include_directories(
    SCION_UTILITIES PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(SCION_UTILITIES PUBLIC SCION_LOGGER SDL2::SDL2)