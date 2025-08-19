add_executable(SCION_CRASH_REPORTER "src/main.cpp")

target_include_directories(
    SCION_CRASH_REPORTER PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	PRIVATE
	../thirdparty/imgui/
	../thirdparty/imgui/imgui_backends
	../thirdparty/imgui/imgui_cpp
)

target_sources(
	SCION_CRASH_REPORTER PRIVATE
	../thirdparty/imgui/imgui.cpp
	../thirdparty/imgui/imgui_demo.cpp
	../thirdparty/imgui/imgui_draw.cpp
	../thirdparty/imgui/imgui_tables.cpp
	../thirdparty/imgui/imgui_widgets.cpp
	../thirdparty/imgui/imgui_backends/imgui_impl_opengl3.cpp
	../thirdparty/imgui/imgui_backends/imgui_impl_sdl2.cpp
	../thirdparty/imgui/imgui_cpp/imgui_stdlib.cpp
)

target_link_libraries(
	SCION_CRASH_REPORTER
	PRIVATE  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
	glad::glad
	soil2
)
