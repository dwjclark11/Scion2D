add_executable(SCION_CRASH_REPORTER "src/main.cpp")

target_include_directories(
    SCION_CRASH_REPORTER PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	PRIVATE
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_backends
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_cpp
)

target_sources(
	SCION_CRASH_REPORTER PRIVATE
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_tables.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_widgets.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_backends/imgui_impl_opengl3.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_backends/imgui_impl_sdl2.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_cpp/imgui_stdlib.cpp
)

target_link_libraries(
	SCION_CRASH_REPORTER
	PRIVATE  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
	glad::glad
	soil2
)
