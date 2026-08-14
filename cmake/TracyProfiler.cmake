if(SCION_ENABLE_TRACY)
	include(FetchContent)
	FetchContent_Declare(
		tracy
		GIT_REPOSITORY	https://github.com/wolfpld/tracy.git
		GIT_TAG			v0.13.1
		GIT_SHALLOW		TRUE
		GIT_PROGRESS	TRUE
	)

	FetchContent_MakeAvailable(tracy)
	target_include_directories(TracyClient PRIVATE ${LUA_INCLUDE_DIR})
endif()
