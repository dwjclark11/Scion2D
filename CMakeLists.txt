cmake_minimum_required(VERSION 3.26)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

project(Scion2D)

if(PROJECT_BINARY_DIR STREQUAL PROJECT_SOURCE_DIR)
    message(
        FATAL_ERROR
        "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# OpenGL realtime debugger settings
set( SCION_OPENGL_DEBUG_CALLBACK					ON	)
set( SCION_OPENGL_DEBUG_FORWARD_COMPATIBILITY		ON	)

# Force discrete GPU on laptops fitted with Optimus / dual GPUs technology
set( SCION_OPENGL_FORCE_DISCRETE_GPU	OFF	)

include(cmake/CompilerSettings.cmake)
include(cmake/Options.cmake)

set(BUILD_SHARED_LIBS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") # .lib, .a
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") # .dll, .so
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") # .exe

if(WIN32)
    add_definitions(-DNOMINMAX)
endif()

find_package(glad CONFIG REQUIRED)
find_package(soil2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(tinyfiledialogs CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
target_link_libraries(sol2 INTERFACE ${LUA_LIBRARIES})

add_subdirectory(SCION_LOGGER)
add_subdirectory(SCION_UTILITIES)
add_subdirectory(SCION_PHYSICS)
add_subdirectory(SCION_RENDERING)
add_subdirectory(SCION_FILESYSTEM)
add_subdirectory(SCION_SOUNDS)
add_subdirectory(SCION_WINDOW)
add_subdirectory(SCION_CORE)
set_target_properties(
    SCION_LOGGER SCION_UTILITIES SCION_PHYSICS
    SCION_RENDERING SCION_FILESYSTEM SCION_SOUNDS SCION_WINDOW SCION_CORE
    PROPERTIES FOLDER "Engine")

include(cmake/AddResources.cmake)
add_subdirectory(SCION_EDITOR)
