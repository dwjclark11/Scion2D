#
#	Compiler settings
#

message( STATUS "Configuring compilers:" )
message( "\t\tc++: " ${CMAKE_CXX_COMPILER_ID} " - " ${CMAKE_CXX_COMPILER_VERSION} )
message( "\t\tc  : " ${CMAKE_C_COMPILER_ID} " - " ${CMAKE_C_COMPILER_VERSION} )

if( WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
	set( CXX_COMPILE_FLAGS
# ------------------------------------ Compiler flags ----------------------------------------------- #
						/std:c++20
						/MP
#						/arch:AVX
#						/arch:AVX2
						/W1					# warning level: /W0, /W1, /W2, /W3, /W4
#						/Wall
#						/WX-				# treat linker warnings as errors
#						/permissive-
#						/fp:except
#						/fp:fast
#						/fp:strict
#						/fp:precise
						/Zc:__cplusplus
#						/Zc:inline
						/Zc:preprocessor	# Preprocessor conformance with c++>=11
#						/Qspectre			# Spectre/Meltdown mitigations. Need specific msvc installation.

# ------------------------------------ Code analysis (slow compilation) ----------------------------- #
#						/external:anglebrackets /external:W0
#						/analyze:WX- /analyze:external- /analyze

# ------------------------------------ Warning suppressions ----------------------------------------- #
#						/wd4061		# switch not handled
#						/wd4062		# switch not handled
#						/wd4244		# possible loss of data
#						/wd4505		# unreferenced function
#						/wd4514		# unreferenced inline function has been removed
#						/wd4599		# precompiled headers
#						/wd4605		# precompiled headers
#						/wd4625		# copy constructor was implicitly defined as deleted
#						/wd4626		# assignement operator was implicitly defined as deleted
#						/wd4623		# default constructor was implicitly defined as deleted
#						/wd4710		# function not inlined
#						/wd4711		# function inlined
#						/wd4715		# return value
#						/wd4723		# divide operation evaluated to zero
#						/wd4820		# padding
#						/wd4866		# left-to-right evaluation order in braced initializer list
#						/wd4868		# left-to-right evaluation order in braced initializer list
#						/wd5027		# move assignement operator was implicitly defined as deleted
#						/wd5045		# Qspectre
#						/wd5219		# implicit conversion
#						/wd5267		# definition of implicit copy constructor/assignment operator is deprecated because it has a user-provided assignment operator/copy constructor
	)
# ------------------------------------ Linker options ----------------------------------------------- #
	set( CMAKE_SHARED_LINKER_FLAGS /manifestuac:no /verbose:lib )
endif()

if( UNIX AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
	set( CXX_COMPILE_FLAGS
						-std=c++20
						-no-pie
						-fno-pie
						-ffunction-sections
						-fdata-sections
						-msse4.1
						-fPIC
						-pthread
#						-W
#						-Wall
#						-Wextra
#						-Werror
#						-Wunused
#						-Wpedantic
#						-Wundef
#						-Wshadow
#						-Wswitch-default
#						-Wswitch-enum
#						-Wconversion
#						-Wunreachable-code
#						-Wdouble-promotion
#						-Wtautological-compare
#						-Wshadow
#						-Wswitch-default
#						-Wswitch-enum
#						-Wunreachable-code
#						-Wdouble-promotion
#						-Walloca
#						-Wno-unused-lambda-capture
#						-Wunused-lambda-capture
#						-Wunused-but-set-variable
#						-Wno-deprecated-copy
#						-Wunused-variable
#						-Wunused-parameter
#						-Wunused-result
#						-Wtype-limits
#						-Wmissing-field-initializers
#						-Wunknown-pragmas
#						-Wreorder
#						-fdiagnostics-color=always
#						-Wno-dev
	)
endif()

if( UNIX AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
	set( CXX_COMPILE_FLAGS
						-std=c++20
						-fno-pie
						-ffunction-sections
						-fdata-sections
						-msse4.1
						-fPIC
						-pthread
#						-W
#						-Wall
#						-Wextra
#						-Werror
#						-Wunused
#						-Wpedantic
#						-Wundef
#						-Wshadow
#						-Wswitch-default
#						-Wswitch-enum
#						-Wconversion
#						-Wunreachable-code
#						-Wdouble-promotion
#						-Wnewline-eof
#						-Wunreachable-code-break
#						-Wclass-varargs
#						-Wstring-conversion
#						-Walloca
#						-Wcomma
#						-Wshorten-64-to-32
#						-Wno-unused-lambda-capture
#						-Wunused-lambda-capture
#						-Wunused-but-set-variable
#						-Wdeprecated-copy
#						-Wunused-variable
#						-Wunused-parameter
#						-Wunused-result
#						-Wtype-limits
#						-Wmissing-field-initializers
#						-Wunknown-pragmas
#						-Wreorder
#						-fdiagnostics-color=always
#						-Wno-dev
	)
endif()

