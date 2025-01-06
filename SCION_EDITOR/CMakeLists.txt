add_executable(SCION_EDITOR
    "src/Application.cpp"
    "src/Application.h"
    "src/main.cpp"

    # Editor Utilities
    "src/editor/utilities/editor_textures.h" 
    "src/editor/utilities/EditorUtilities.h"
	"src/editor/utilities/ImGuiUtils.h"
	"src/editor/utilities/ImGuiUtils.cpp"
	"src/editor/utilities/EditorFramebuffers.h"
	"src/editor/utilities/DrawComponentUtils.h"
	"src/editor/utilities/DrawComponentUtils.cpp"
	"src/editor/utilities/fonts/IconsFontAwesome5.h"

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
    "src/editor/displays/AssetDisplay.h" 
    "src/editor/displays/AssetDisplay.cpp" 
	"src/editor/displays/MenuDisplay.h"
	"src/editor/displays/MenuDisplay.cpp"
	"src/editor/displays/TileDetailsDisplay.h"
	"src/editor/displays/TileDetailsDisplay.cpp"
	"src/editor/displays/SceneHierarchyDisplay.h"
	"src/editor/displays/SceneHierarchyDisplay.cpp"
	"src/editor/displays/EditorStyleToolDisplay.h"
	"src/editor/displays/EditorStyleToolDisplay.cpp"

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
    "src/editor/tools/AbstractTool.cpp"
    "src/editor/tools/TileTool.h"
    "src/editor/tools/TileTool.cpp"
	"src/editor/tools/CreateTileTool.h"
	"src/editor/tools/CreateTileTool.cpp"
	"src/editor/tools/ToolAccessories.h"
	"src/editor/tools/ToolManager.h"
	"src/editor/tools/ToolManager.cpp"
	"src/editor/tools/RectFillTool.h"
	"src/editor/tools/RectFillTool.cpp"

	# Gizmos
	"src/editor/tools/gizmos/Gizmo.h"
	"src/editor/tools/gizmos/Gizmo.cpp"

	# Commands
	"src/editor/commands/AbstractCommandManager.h"
	"src/editor/commands/AbstractCommandManager.inl"
	"src/editor/commands/CommandManager.h"
	"src/editor/commands/cmds/CreateTileToolCmds.h"
	"src/editor/commands/cmds/CreateTileToolCmds.cpp"
	"src/editor/commands/cmds/RectToolCmds.h"
	"src/editor/commands/cmds/RectToolCmds.cpp"
)

target_include_directories(
    SCION_EDITOR PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(SCION_EDITOR PRIVATE SCION_CORE imgui::imgui)

add_resources(TARGET Assets IN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets OUT_DIR assets)
add_dependencies(SCION_EDITOR Copy-Assets)

set_target_properties(SCION_EDITOR PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")

target_compile_options(
    SCION_EDITOR PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)
