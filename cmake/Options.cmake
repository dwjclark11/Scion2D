#
#	Options
#

#--------------- Preprocessor definitions -------------------#
if(SCION_OPENGL_DEBUG_CALLBACK)
	add_compile_definitions(SCION_OPENGL_DEBUG_CALLBACK)
endif()

if(SCION_OPENGL_DEBUG_CALLBACK_FWD_COMPATIBILITY)
	add_compile_definitions(SCION_OPENGL_DEBUG_CALLBACK_FWD_COMPATIBILITY)
endif()

if(SCION_OPENGL_DEBUG_BREAKONERROR)
	add_compile_definitions(SCION_OPENGL_DEBUG_BREAKONERROR)
endif()

if(SCION_OPENGL_DEBUG_BREAKONWARNING)
	add_compile_definitions(SCION_OPENGL_DEBUG_BREAKONWARNING)
endif()

if(SCION_OPENGL_FORCE_DISCRETE_GPU)
	add_compile_definitions(SCION_OPENGL_FORCE_DISCRETE_GPU)
endif()
