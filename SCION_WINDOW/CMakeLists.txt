add_library(SCION_WINDOW
    "include/Windowing/Inputs/Button.h"
    "src/Button.cpp"
    "include/Windowing/Inputs/Gamepad.h"
    "src/Gamepad.cpp"
    "include/Windowing/Inputs/GPButtons.h"
    "include/Windowing/Inputs/Keyboard.h"
    "src/Keyboard.cpp"
    "include/Windowing/Inputs/Keys.h"
    "include/Windowing/Inputs/Mouse.h"
    "src/Mouse.cpp"
    "include/Windowing/Inputs/MouseButtons.h"

    "include/Windowing/Window/Window.h"
    "src/Window.cpp")

target_include_directories(
    SCION_WINDOW PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(SCION_WINDOW PUBLIC SCION_UTILITIES)