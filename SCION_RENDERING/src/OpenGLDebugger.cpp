#include "Rendering/Utils/OpenGLDebugger.h"

#include <set>
#include <sstream>
#include <glad/glad.h>

#include "Logger/Logger.h"

static void glDebugCallback( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
							 const char* message, const void* userParam );

#ifdef __linux
#include <signal.h>
#endif

namespace SCION_RENDERING
{
OpenGLDebugger::OpenGLDebugger()
{
}

OpenGLDebugger::~OpenGLDebugger()
{
}

void OpenGLDebugger::init()
{
	SCION_LOG( "Setting up OpenGL debug callback (requires OpenGL version > 4.3.)" );

	{ // Checking hardware adapter
		auto gl_vendor = ( "\tGL_VENDOR..................: " +
						   std::string( reinterpret_cast<const char*>( ::glGetString( GL_VENDOR ) ) ) );
		auto gl_renderer = ( "\tGL_RENDERER................: " +
							 std::string( reinterpret_cast<const char*>( ::glGetString( GL_RENDERER ) ) ) );
		auto gl_version = ( "\tGL_VERSION.................: " +
							std::string( reinterpret_cast<const char*>( ::glGetString( GL_VERSION ) ) ) );
		auto gl_shading_language =
			( "\tGL_SHADING_LANGUAGE_VERSION: " +
			  std::string( reinterpret_cast<const char*>( ::glGetString( GL_SHADING_LANGUAGE_VERSION ) ) ) );

		/*	SCION_LOG( gl_vendor );
			SCION_LOG( gl_renderer );
			SCION_LOG( gl_version );
			SCION_LOG( gl_shading_language );*/

		std::stringstream ss;
		// clang-format off
		ss  <<"Found following graphics adapter:\n"
			<< gl_vendor   << "\n"
			<< gl_renderer << "\n"
			<< gl_version  << "\n"
			<< gl_shading_language;
		// clang-format on

		SCION_LOG( ss.str() );
	}

	{ // Checking OpenGL version
		GLint versionMajor{};
		::glGetIntegerv( GL_MAJOR_VERSION, &versionMajor );

		GLint versionMinor{};
		::glGetIntegerv( GL_MINOR_VERSION, &versionMinor );

		auto version = 10 * versionMajor + versionMinor;

		if ( version < 43 )
		{
			SCION_WARN( "OpenGL version < 4.3...abort" );
			return;
		}
		else
		{
			SCION_LOG( "All good, OpenGL debug callback set properly" );
		}
	}

	{ // Setup callback
		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageCallback( glDebugCallback, NULL );
		glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE );
	}
}
} // namespace SCION_RENDERING

void glDebugCallback( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message,
					  const void* userParam )
{
	(void)length;
	(void)userParam;

	std::set<unsigned int> ignore{
		// clang-format off
			131169, // The driver allocated storage for renderbuffer 1
			131185, // Buffer object 4 (bound to GL_ELEMENT_ARRAY_BUFFER_ARB, usage hint is GL_STATIC_DRAW) will use VIDEO memory as the source for buffer object operations
			131204, // Texture state usage warning: The texture object (0) bound to texture image unit 0 does not have a defined base level and cannot be used for texture mapping.
			131218	// Program/shader state performance warning: Vertex shader in program 3 is being recompiled based on GL state.
		// clang-format on
	};

	if ( ignore.contains( id ) )
	{
		return;
	}

	const char* msgSource = nullptr;
	switch ( source )
	{
	case GL_DEBUG_SOURCE_API: msgSource = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: msgSource = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: msgSource = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: msgSource = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: msgSource = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: msgSource = "Other"; break;
	default: break;
	}

	const char* errorType = nullptr;
	switch ( type )
	{
	case GL_DEBUG_TYPE_ERROR: errorType = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: errorType = "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: errorType = "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY: errorType = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE: errorType = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER: errorType = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP: errorType = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP: errorType = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER: errorType = "Other"; break;
	default: break;
	}

	const char* errorSeverity = nullptr;
	switch ( severity )
	{
	case GL_DEBUG_SEVERITY_HIGH: errorSeverity = "High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: errorSeverity = "Medium"; break;
	case GL_DEBUG_SEVERITY_LOW: errorSeverity = "Low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: errorSeverity = "Notification"; break;
	default: break;
	}

	std::stringstream ss;
	// clang-format off
	ss
	<< "---> OpenGL Debug Callback <---"
	<< "\n\t\tSource..: " << msgSource
	<< "\n\t\tType....: " << errorType
	<< "\n\t\tSeverity: " << errorSeverity
	<< "\n\t\tId......: " << id
	<< "\n\t\tMessage.: " << message;

	switch ( severity )
	{
	case GL_DEBUG_SEVERITY_HIGH:
		SCION_LOGGER::Logger::GetInstance().Error( ss.str() );
		#ifdef SCION_OPENGL_DEBUG_BREAKONERROR
			#ifdef _WIN32
						__debugbreak();
			#elif __linux
						raise( SIGTRAP );
			#endif
		#endif
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
	case GL_DEBUG_SEVERITY_LOW:
		SCION_LOGGER::Logger::GetInstance().Warn( ss.str() );
		#ifdef SCION_OPENGL_DEBUG_BREAKONWARNING
			#ifdef _WIN32
						__debugbreak();
			#elif __linux
						raise( SIGTRAP );
			#endif
		#endif
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		SCION_LOGGER::Logger::GetInstance().Log( ss.str() );
		break;
	default:
		break;
	}
	// clang-format on
}
