
if(WIN32)
	set(FILE_PROCESSOR_PATH "src/FileProcessor_Win.cpp")
else()
	set(FILE_PROCESSOR_PATH "src/FileProcessor_Unix.cpp")
endif()

add_library(SCION_FILESYSTEM
	# Dialogs
	"include/ScionFilesystem/Dialogs/FileDialog.h"
	"src/FileDialog.cpp"

	# Serializers
	"include/ScionFilesystem/Serializers/JSONSerializer.h"
	"include/ScionFilesystem/Serializers/JSONSerializer.inl"
	"src/JSONSerializer.cpp"
	"include/ScionFilesystem/Serializers/LuaSerializer.h"
	"include/ScionFilesystem/Serializers/LuaSerializer.inl"
	"src/LuaSerializer.cpp"

	# Process
	"include/ScionFilesystem/Process/FileProcessor.h"
	${FILE_PROCESSOR_PATH}

	# Utilities
	"include/ScionFilesystem/Utilities/DirectoryWatcher.h"
	"src/DirectoryWatcher.cpp"
)

target_include_directories(
	SCION_FILESYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
	SCION_FILESYSTEM PRIVATE SCION_LOGGER tinyfiledialogs::tinyfiledialogs rapidjson SCION_UTILITIES
)

target_compile_options(
	SCION_FILESYSTEM PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
