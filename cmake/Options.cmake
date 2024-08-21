#
#	Options
#

#--------------- Preprocessor definitions -------------------#
if(SCION_OPENGL_DEBUG_CALLBACK)
	add_compile_definitions(SCION_OPENGL_DEBUG_CALLBACK)
	message ( "Scion engine will be built with embedded realtime OpenGL debugger." )
endif()

if(SCION_OPENGL_DEBUG_FORWARD_COMPATIBILITY)
	add_compile_definitions(SCION_OPENGL_DEBUG_FORWARD_COMPATIBILITY)
	message ( "Scion engine will be build with a strict OpenGL context. This will trigger warnings on deprecated OpenGL dunctions." )
endif()

if(SCION_OPENGL_FORCE_DISCRETE_GPU)
	add_compile_definitions(SCION_OPENGL_FORCE_DISCRETE_GPU)
	message ( "Scion engine will attempt to force discrete GPU on optimus laptops." )
endif()
