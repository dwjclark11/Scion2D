add_library(SCION_PHYSICS
    "include/Physics/Box2DWrappers.h"
    "src/Box2DWrappers.cpp"
    "include/Physics/ContactListener.h"
    "src/ContactListener.cpp"
    "include/Physics/UserData.h"
    "src/UserData.cpp"
	"include/Physics/PhysicsUtilities.h"
	"src/PhysicsUtilities.cpp"
)

target_include_directories(
    SCION_PHYSICS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(SCION_PHYSICS
    PRIVATE SCION_LOGGER
	PRIVATE SCION_UTILITIES
    PUBLIC box2d::box2d)

target_compile_options(
    SCION_PHYSICS PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)
