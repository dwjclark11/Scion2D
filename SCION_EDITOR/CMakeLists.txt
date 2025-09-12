find_package(tinyxml2 CONFIG REQUIRED)

add_executable(SCION_EDITOR
    "src/Application.cpp"
    "src/Application.h"
    "src/main.cpp"

    # Editor Utilities
	"src/editor/utilities/fonts/editor_fonts.h"
	"src/editor/utilities/fonts/editor_fonts.cpp"
    "src/editor/utilities/editor_textures.h"
	"src/editor/utilities/editor_textures.cpp"
    "src/editor/utilities/EditorUtilities.h"
    "src/editor/utilities/EditorUtilities.cpp"
	"src/editor/utilities/imgui/ImGuiUtils.h"
	"src/editor/utilities/imgui/ImGuiUtils.cpp"
	"src/editor/utilities/EditorFramebuffers.h"
	"src/editor/utilities/DrawComponentUtils.h"
	"src/editor/utilities/DrawComponentUtils.cpp"
	"src/editor/utilities/fonts/IconsFontAwesome5.h"
	"src/editor/utilities/imgui/Gui.h"
	"src/editor/utilities/imgui/Gui.cpp"
	"src/editor/utilities/EditorState.h"
	"src/editor/utilities/EditorState.cpp"

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
	"src/editor/displays/ContentDisplay.h"
	"src/editor/displays/ContentDisplay.cpp"
	"src/editor/displays/AssetDisplayUtils.h"
	"src/editor/displays/AssetDisplayUtils.cpp"
	"src/editor/displays/ScriptDisplay.h"
	"src/editor/displays/ScriptDisplay.cpp"
	"src/editor/displays/PackageDisplay.h"
	"src/editor/displays/PackageDisplay.cpp"
	"src/editor/displays/ProjectSettingsDisplay.h"
	"src/editor/displays/ProjectSettingsDisplay.cpp"
	
    # Editor Systems
    "src/editor/systems/GridSystem.h" 
    "src/editor/systems/GridSystem.cpp" 
    "src/editor/systems/EditorRenderSystem.h"
	"src/editor/systems/EditorRenderSystem.cpp"
	
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
	"src/editor/tools/gizmos/TranslateGizmo.h"
	"src/editor/tools/gizmos/TranslateGizmo.cpp"
	"src/editor/tools/gizmos/ScaleGizmo.h"
	"src/editor/tools/gizmos/ScaleGizmo.cpp"
	"src/editor/tools/gizmos/RotateGizmo.h"
	"src/editor/tools/gizmos/RotateGizmo.cpp"

	# Commands
	"src/editor/commands/AbstractCommandManager.h"
	"src/editor/commands/AbstractCommandManager.inl"
	"src/editor/commands/CommandManager.h"
	"src/editor/commands/cmds/CreateTileToolCmds.h"
	"src/editor/commands/cmds/CreateTileToolCmds.cpp"
	"src/editor/commands/cmds/RectToolCmds.h"
	"src/editor/commands/cmds/RectToolCmds.cpp"
	"src/editor/commands/cmds/TileCmds.h"
	"src/editor/commands/cmds/TileCmds.cpp"
	
	# Events
	"src/editor/events/EditorEventTypes.h"	
	"src/editor/events/EditorEventTypes.cpp"

	"src/editor/hub/Hub.cpp"
	"src/editor/hub/Hub.h"
	
	# Loaders
	"src/editor/loaders/ProjectLoader.h"
	"src/editor/loaders/ProjectLoader.cpp"

	${APP_ICON_RESOURCE}
	"src/editor/scripting/EditorCoreLuaWrappers.h"
	"src/editor/scripting/EditorCoreLuaWrappers.cpp"
	
	# Packaging
	"src/editor/packaging/ScriptCompiler.h"
	"src/editor/packaging/ScriptCompiler.cpp"
	"src/editor/packaging/IconReplacer.h"
	"src/editor/packaging/IconReplacer.cpp"
	"src/editor/packaging/AssetPackager.h"
	"src/editor/packaging/AssetPackager.cpp"
	"src/editor/packaging/Packager.h"
	"src/editor/packaging/Packager.cpp"
 "src/editor/loaders/TiledMapImporter.h" "src/editor/loaders/TiledMapImporter.cpp")

target_compile_definitions(SCION_EDITOR PUBLIC IN_SCION_EDITOR)

target_sources(
	SCION_EDITOR PRIVATE
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_tables.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_widgets.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_backends/imgui_impl_opengl3.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_backends/imgui_impl_sdl2.cpp
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_cpp/imgui_stdlib.cpp
)

target_include_directories(
    SCION_EDITOR PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	PRIVATE
	${PROJECT_SOURCE_DIR}/thirdparty/imgui
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_backends
	${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_cpp
)

target_link_libraries(
	SCION_EDITOR
	PRIVATE SCION_CORE_EDITOR
	libzippp::libzippp
	tinyxml2::tinyxml2
)

set_target_properties(SCION_EDITOR PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")

target_compile_options(
    SCION_EDITOR PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)

target_precompile_headers(SCION_EDITOR REUSE_FROM PCH)
