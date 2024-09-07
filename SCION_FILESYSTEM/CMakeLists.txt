add_library(SCION_FILESYSTEM
	# Dialogs
	"include/ScionFilesystem/Dialogs/FileDialog.h"
	"src/FileDialog.cpp"
)

target_include_directories(
	SCION_FILESYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SCION_FILESYSTEM PRIVATE SCION_LOGGER tinyfiledialogs::tinyfiledialogs)

target_compile_options(
	SCION_FILESYSTEM PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
