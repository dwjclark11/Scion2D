add_subdirectory(glad)
add_subdirectory(soil)
add_subdirectory(imgui)

set_target_properties(glad soil imgui PROPERTIES FOLDER "Dependencies")