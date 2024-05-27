add_library(SCION_CORE
    "include/Core/CoreUtilities/CoreEngineData.h"
    "src/CoreUtilities/CoreEngineData.cpp"
    "include/Core/CoreUtilities/CoreUtilities.h"
    "src/CoreUtilities/CoreUtilities.cpp"
    "include/Core/CoreUtilities/FollowCamera.h"
    "src/CoreUtilities/FollowCamera.cpp"

    "include/Core/ECS/Entity.h"
    "include/Core/ECS/Entity.inl"
    "src/ECS/Entity.cpp"
    "include/Core/ECS/MetaUtilities.h"
    "src/ECS/MetaUtilities.cpp"
    "include/Core/ECS/Registry.h"
    "include/Core/ECS/Registry.inl"
    "src/ECS/Registry.cpp"

    "include/Core/ECS/Components/AllComponents.h"
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

    "include/Core/Resources/AssetManager.h"
    "src/Resources/AssetManager.cpp"

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

    "include/Core/States/State.h"
    "src/States/State.cpp"
    "include/Core/States/StateStack.h"
    "src/States/StateStack.cpp"

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
    "src/Systems/ScriptingSystem.cpp")

target_include_directories(
    SCION_CORE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(SCION_CORE PUBLIC
    glm::glm EnTT::EnTT SDL2::SDL2 sol2
    SCION_LOGGER SCION_UTILITIES SCION_PHYSICS SCION_RENDERING SCION_SOUNDS SCION_WINDOW)