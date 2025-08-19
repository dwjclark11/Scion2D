
if (MSVC)
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:utf-8>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>")
endif()

add_library(PCH STATIC "pch/pch.cpp")

target_compile_options( 
    PCH PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
