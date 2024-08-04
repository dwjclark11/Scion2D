add_executable(SCION_EDITOR
    "src/Application.cpp"
    "src/Application.h"
    "src/main.cpp"

    # Editor Utilities
    "src/editor/utilities/editor_textures.h" 
    "src/editor/utilities/EditorUtilities.h"

    # Editor Displays
    "src/editor/displays/IDisplay.h"
    "src/editor/displays/LogDisplay.h"
    "src/editor/displays/LogDisplay.cpp"
    "src/editor/displays/SceneDisplay.h"
    "src/editor/displays/SceneDisplay.cpp"  
    "src/editor/displays/TilesetDisplay.h"
    "src/editor/displays/TilesetDisplay.cpp"
    "src/editor/displays/TilemapDisplay.h" 
    "src/editor/displays/TilemapDisplay.cpp" 
    "src/editor/utilities/EditorFramebuffers.h"
    "src/editor/displays/AssetDisplay.h" 
    "src/editor/displays/AssetDisplay.cpp" 

    # Editor Systems
    "src/editor/systems/GridSystem.h" 
    "src/editor/systems/GridSystem.cpp" 
    
    # Scenes
    "src/editor/scene/SceneObject.h" 
    "src/editor/scene/SceneObject.cpp" 
    "src/editor/scene/SceneManager.h" 
    "src/editor/scene/SceneManager.cpp"
    
    # Tools
    "src/editor/tools/AbstractTool.h"
    "src/editor/tools/AbstractTool.cpp")

target_include_directories(
    SCION_EDITOR PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(SCION_EDITOR PRIVATE SCION_CORE imgui::imgui)

add_resources(TARGET Assets IN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets OUT_DIR assets)
add_dependencies(SCION_EDITOR Copy-Assets)

set_target_properties(SCION_EDITOR PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")

target_compile_options(
    SCION_EDITOR PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)
