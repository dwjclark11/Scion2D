add_library(SCION_LOGGER
    "include/Logger/Logger.h"
    "include/Logger/Logger.inl"
    "src/Logger.cpp"
	"include/Logger/CrashLogger.h"
	"src/CrashLogger.cpp"
)

target_include_directories(
    SCION_LOGGER PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries( SCION_LOGGER PUBLIC fmt::fmt)

target_compile_options( 
    SCION_LOGGER PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)

target_precompile_headers(SCION_LOGGER REUSE_FROM PCH)
