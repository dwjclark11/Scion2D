find_package(tinyxml2 CONFIG REQUIRED)

file(GLOB_RECURSE CMD_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/commands/*.cpp)
file(GLOB_RECURSE DISPLAY_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/displays/*.cpp)
file(GLOB_RECURSE EVENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/events/*.cpp)
file(GLOB_RECURSE HUB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/hub/*.cpp)
file(GLOB_RECURSE LOADERS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/loaders/*.cpp)
file(GLOB_RECURSE PACKAGING_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/packaging/*.cpp)
file(GLOB_RECURSE SCENE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/scene/*.cpp)
file(GLOB_RECURSE SCRIPT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/scripting/*.cpp)
file(GLOB_RECURSE SYSTEMS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/systems/*.cpp)
file(GLOB_RECURSE TOOLS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/tools/*.cpp)
file(GLOB_RECURSE UTILITIES_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/utilities/*.cpp)

add_executable(SCION_EDITOR
    "src/Application.cpp"
    "src/Application.h"
    "src/main.cpp"

    ${CMD_SRC}
	${DISPLAY_SRC}
	${EVENT_SRC}
	${HUB_SRC}
	${LOADERS_SRC}
	${PACKAGING_SRC}
	${SCENE_SRC}
	${SCRIPT_SRC}
	${SYSTEMS_SRC}
	${TOOLS_SRC}
	${UTILITIES_SRC}
)

target_compile_definitions(SCION_EDITOR PUBLIC IN_SCION_EDITOR)

target_include_directories( SCION_EDITOR PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )

target_include_directories(
    SCION_EDITOR PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
	SCION_EDITOR
	PRIVATE SCION_CORE_EDITOR
	libzippp::libzippp
	tinyxml2::tinyxml2
	imgui
)

set_target_properties(SCION_EDITOR PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")

target_compile_options(
    SCION_EDITOR PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)

target_precompile_headers(SCION_EDITOR REUSE_FROM PCH)
