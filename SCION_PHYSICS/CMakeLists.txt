include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# Prefer static linking (avoids DLL issues)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)

# 1. Declare the Box2D dependency
FetchContent_Declare(
  box2d
  GIT_REPOSITORY https://github.com/erincatto/box2d.git
  GIT_TAG        v2.4.2 # Or use a specific release tag like v2.4.1
)

# 2. Make content available (downloads and adds to build)
FetchContent_MakeAvailable(box2d)

add_library(SCION_PHYSICS
    "include/Physics/Box2DWrappers.h"
    "src/Box2DWrappers.cpp"
    "include/Physics/ContactListener.h"
    "src/ContactListener.cpp"
    "include/Physics/UserData.h"
    "src/UserData.cpp"
	"include/Physics/PhysicsUtilities.h"
	"src/PhysicsUtilities.cpp"
	"include/Physics/BoxTraceCallback.h"
 "src/BoxTraceCallback.cpp" "include/Physics/RayCastCallback.h" "src/RayCastCallback.cpp")

target_include_directories(
    SCION_PHYSICS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(SCION_PHYSICS
    PRIVATE SCION_LOGGER
	PRIVATE SCION_UTILITIES
    PUBLIC box2d
)

target_compile_options(
    SCION_PHYSICS PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)

target_precompile_headers(SCION_PHYSICS REUSE_FROM PCH)
