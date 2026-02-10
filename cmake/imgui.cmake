include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG        docking
  GIT_SHALLOW 1
)

FetchContent_MakeAvailable(imgui)
 
file(
	GLOB IM_SOURCES
	${imgui_SOURCE_DIR}/*.cpp
	# .cpp files for backends
	${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IM_SOURCES} )

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/misc/cpp
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    glad::glad
)
