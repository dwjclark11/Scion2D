add_library(SCION_UTILITIES
    "include/ScionUtilities/RandomGenerator.h"
    "src/RandomGenerator.cpp"
    "include/ScionUtilities/SDL_Wrappers.h"
    "src/SDL_Wrappers.cpp"
    "include/ScionUtilities/Timer.h"
    "src/Timer.cpp" 
    "include/ScionUtilities/ScionUtilities.h"
	"src/ScionUtilities.cpp"
	"include/ScionUtilities/HelperUtilities.h"
)

target_include_directories(
    SCION_UTILITIES PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(SCION_UTILITIES PUBLIC SCION_LOGGER SDL2::SDL2)

target_compile_options(
    SCION_UTILITIES PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)
