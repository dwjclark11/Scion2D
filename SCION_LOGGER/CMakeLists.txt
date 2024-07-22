add_library(SCION_LOGGER
    "include/Logger/Logger.h"
    "include/Logger/Logger.inl"
    "src/Logger.cpp")

target_include_directories(
    SCION_LOGGER PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries( SCION_LOGGER PUBLIC fmt::fmt)
