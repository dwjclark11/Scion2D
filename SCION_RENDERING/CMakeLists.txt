add_library(SCION_RENDERING
    "include/Rendering/Utils/OpenGLDebugger.h"
    "src/OpenGLDebugger.cpp"

    "include/Rendering/Buffers/Framebuffer.h"
    "src/Framebuffer.cpp"

    "include/Rendering/Core/Batcher.h"
    "include/Rendering/Core/BatchRenderer.h"
    "src/BatchRenderer.cpp"
    "include/Rendering/Core/Camera2D.h"
    "src/Camera2D.cpp"
    "include/Rendering/Core/CircleBatchRenderer.h"
    "src/CircleBatchRenderer.cpp"
    "include/Rendering/Core/LineBatchRenderer.h"
    "src/LineBatchRenderer.cpp"
    "include/Rendering/Core/RectBatchRenderer.h"
    "src/RectBatchRenderer.cpp"
    "include/Rendering/Core/Renderer.h"
    "src/Renderer.cpp"
    "include/Rendering/Core/TextBatchRenderer.h"
    "src/TextBatchRenderer.cpp"

    "include/Rendering/Essentials/BatchTypes.h"
    "include/Rendering/Essentials/Font.h"
    "src/Font.cpp"
    "include/Rendering/Essentials/FontLoader.h"
    "src/FontLoader.cpp"
    "include/Rendering/Essentials/Primitives.h"
    "include/Rendering/Essentials/Shader.h"
    "src/Shader.cpp"
    "include/Rendering/Essentials/ShaderLoader.h"
    "src/ShaderLoader.cpp"
    "include/Rendering/Essentials/Texture.h"
    "src/Texture.cpp"
    "include/Rendering/Essentials/TextureLoader.h"
    "src/TextureLoader.cpp"
    "include/Rendering/Essentials/Vertex.h")

target_include_directories(
    SCION_RENDERING PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(SCION_RENDERING
    PRIVATE SCION_LOGGER
    PUBLIC glm::glm glad::glad soil2)

target_compile_options(
    SCION_RENDERING PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)
