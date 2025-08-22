add_executable(
	SCION_ENGINE

	"src/main.cpp"
	"src/Runtime.h"
	"src/Runtime.cpp"
)


target_link_libraries(SCION_ENGINE
	PRIVATE SCION_CORE
	libzippp::libzippp
)

target_compile_options(
    SCION_ENGINE
	PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)

target_precompile_headers(SCION_ENGINE REUSE_FROM PCH)
