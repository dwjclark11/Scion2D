
file( GLOB_RECURSE RENDERING_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )

add_library(SCION_RENDERING ${RENDERING_SRC} )

target_include_directories(
    SCION_RENDERING PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${SOL2_INCLUDE_DIRS}
)

target_link_libraries(SCION_RENDERING
    PRIVATE
	SCION_LOGGER
    glm::glm
	glad::glad
	soil2
	sol2::sol2
	${LUA_LIBRARIES}
)

target_compile_options(
    SCION_RENDERING PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)

target_precompile_headers(SCION_RENDERING REUSE_FROM PCH)
