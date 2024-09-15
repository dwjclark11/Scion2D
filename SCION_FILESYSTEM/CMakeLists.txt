add_library(SCION_FILESYSTEM
	# Dialogs
	"include/ScionFilesystem/Dialogs/FileDialog.h"
	"src/FileDialog.cpp"

	# Serializers
	"include/ScionFilesystem/Serializers/JSONSerializer.h"
	"include/ScionFilesystem/Serializers/JSONSerializer.inl"
	"src/JSONSerializer.cpp"
)

target_include_directories(
	SCION_FILESYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SCION_FILESYSTEM PRIVATE SCION_LOGGER tinyfiledialogs::tinyfiledialogs rapidjson)

target_compile_options(
	SCION_FILESYSTEM PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
