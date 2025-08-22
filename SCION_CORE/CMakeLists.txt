find_package(SDL2_image REQUIRED)

set(COMMON_CORE_SRC
	# Core Utilities
    "include/Core/CoreUtilities/CoreEngineData.h"
    "src/CoreUtilities/CoreEngineData.cpp"
    "include/Core/CoreUtilities/CoreUtilities.h"
    "src/CoreUtilities/CoreUtilities.cpp"
    "include/Core/CoreUtilities/FollowCamera.h"
    "src/CoreUtilities/FollowCamera.cpp"
	"include/Core/CoreUtilities/EngineShaders.h"
	"include/Core/CoreUtilities/ProjectInfo.h"
	"src/CoreUtilities/ProjectInfo.cpp"
	"include/Core/CoreUtilities/Prefab.h"
	"src/CoreUtilities/Prefab.cpp"

	# ECS - Entity, Registry
    "include/Core/ECS/Entity.h"
    "include/Core/ECS/Entity.inl"
    "src/ECS/Entity.cpp"
    "include/Core/ECS/MetaUtilities.h"
    "src/ECS/MetaUtilities.cpp"
    "include/Core/ECS/Registry.h"
    "include/Core/ECS/Registry.inl"
    "src/ECS/Registry.cpp"
    "include/Core/ECS/MainRegistry.h" 
    "src/ECS/MainRegistry.cpp"
	"include/Core/ECS/ECSUtils.h"
	"src/ECS/ECSUtils.cpp"

    # Components
    "include/Core/ECS/Components/Identification.h"
    "include/Core/ECS/Components/AnimationComponent.h"
    "src/ECS/Components/AnimationComponent.cpp"
    "include/Core/ECS/Components/BoxColliderComponent.h"
    "src/ECS/Components/BoxColliderComponent.cpp"
    "include/Core/ECS/Components/CircleColliderComponent.h"
    "src/ECS/Components/CircleColliderComponent.cpp"
    "include/Core/ECS/Components/PhysicsComponent.h"
    "src/ECS/Components/PhysicsComponent.cpp"
    "include/Core/ECS/Components/RigidBodyComponent.h"
    "src/ECS/Components/RigidBodyComponent.cpp"
    "include/Core/ECS/Components/ScriptComponent.h"
    "include/Core/ECS/Components/SpriteComponent.h"
    "src/ECS/Components/SpriteComponent.cpp"
    "include/Core/ECS/Components/TextComponent.h"
    "src/ECS/Components/TextComponent.cpp"
    "include/Core/ECS/Components/TransformComponent.h"
    "src/ECS/Components/TransformComponent.cpp"
    "include/Core/ECS/Components/TileComponent.h"
	"include/Core/ECS/Components/Relationship.h"
	"src/ECS/Components/Relationship.cpp"
	"include/Core/ECS/Components/UIComponent.h"
	"src/ECS/Components/UIComponent.cpp"

	"include/Core/ECS/Components/AllComponents.h"

	# Character
	"include/Core/Character/Character.h"
	"src/Character/Character.cpp"
	"include/Core/Character/PlayerStart.h"
	"src/Character/PlayerStart.cpp"

	# Resources
    "include/Core/Resources/AssetManager.h"
	"src/Resources/AssetManager.cpp"
    "include/Core/Resources/fonts/default_fonts.h"
	"src/Resources/fonts/default_fonts.cpp"
    
	# Scripting
    "include/Core/Scripting/ContactListenerBind.h"
    "src/Scripting/ContactListenerBind.cpp"
    "include/Core/Scripting/GlmLuaBindings.h"
    "src/Scripting/GlmLuaBindings.cpp"
    "include/Core/Scripting/InputManager.h"
    "src/Scripting/InputManager.cpp"
    "include/Core/Scripting/RendererBindings.h"
    "src/Scripting/RendererBindings.cpp"
    "include/Core/Scripting/SoundBindings.h"
    "src/Scripting/SoundBindings.cpp"
    "include/Core/Scripting/UserDataBindings.h"
    "include/Core/Scripting/UserDataBindings.inl"
    "src/Scripting/UserDataBindings.cpp"
	"include/Core/Scripting/LuaFilesystemBindings.h"
	"src/Scripting/LuaFilesystemBindings.cpp"
	"include/Core/Scripting/CrashLoggerTestBindings.h"
	"src/Scripting/CrashLoggerTestBindings.cpp"
	"include/Core/Scripting/ScriptingUtilities.h"
	"src/Scripting/ScriptingUtilities.cpp"

	# States and State Machine
    "include/Core/States/State.h"
    "src/States/State.cpp"
    "include/Core/States/StateStack.h"
    "src/States/StateStack.cpp"
    "include/Core/States/StateMachine.h"
    "src/States/StateMachine.cpp"

	# Core Systems
    "include/Core/Systems/AnimationSystem.h"
    "src/Systems/AnimationSystem.cpp"
    "include/Core/Systems/PhysicsSystem.h"
    "src/Systems/PhysicsSystem.cpp"
    "include/Core/Systems/RenderShapeSystem.h"
    "src/Systems/RenderShapeSystem.cpp"
    "include/Core/Systems/RenderSystem.h"
    "src/Systems/RenderSystem.cpp"
    "include/Core/Systems/RenderUISystem.h"
    "src/Systems/RenderUISystem.cpp"
    "include/Core/Systems/ScriptingSystem.h"
    "src/Systems/ScriptingSystem.cpp"
	"include/Core/Systems/RenderPickingSystem.h"
	"src/Systems/RenderPickingSystem.cpp"

	# Loaders and Serializers
	"include/Core/ECS/Components/ComponentSerializer.h"
	"include/Core/ECS/Components/ComponentSerializer.inl"
	"src/ECS/Components/ComponentSerializer.cpp"
	"include/Core/Loaders/TilemapLoader.h"
	"src/Loaders/TilemapLoader.cpp"

	# Events
	"include/Core/Events/EventDispatcher.h"
	"src/Events/EventDispatcher.cpp"
	"include/Core/Events/EngineEventTypes.h"
	"src/Events/EngineEventTypes.cpp"
	
	# Scenes
	"include/Core/Scene/Scene.h"
	"src/Scene/Scene.cpp"
	"include/Core/Scene/SceneManager.h"
	"src/Scene/SceneManager.cpp"	
)

add_library( SCION_CORE ${COMMON_CORE_SRC} )

target_include_directories(
    SCION_CORE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${SOL2_INCLUDE_DIRS}
)

target_precompile_headers(SCION_CORE REUSE_FROM PCH)

target_link_libraries(SCION_CORE PUBLIC
    glm::glm EnTT::EnTT sol2::sol2 ${LUA_LIBRARIES}
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    SCION_LOGGER SCION_UTILITIES SCION_PHYSICS
	SCION_RENDERING SCION_FILESYSTEM SCION_SOUNDS SCION_WINDOW)

target_compile_options(
    SCION_CORE PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)


## SCION_CORE_EDITOR - Editor Compile Flag is set
add_library( SCION_CORE_EDITOR ${COMMON_CORE_SRC} )

target_compile_definitions(SCION_CORE_EDITOR PUBLIC IN_SCION_EDITOR)

target_include_directories(
    SCION_CORE_EDITOR PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${SOL2_INCLUDE_DIRS}
)

target_precompile_headers(SCION_CORE_EDITOR REUSE_FROM PCH)

target_link_libraries(SCION_CORE_EDITOR PUBLIC
    glm::glm EnTT::EnTT sol2::sol2 ${LUA_LIBRARIES}
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
	$<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    SCION_LOGGER SCION_UTILITIES SCION_PHYSICS
	SCION_RENDERING SCION_FILESYSTEM SCION_SOUNDS SCION_WINDOW)

target_compile_options(
    SCION_CORE_EDITOR PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)
