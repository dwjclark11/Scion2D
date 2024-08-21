#include "Rendering/Utils/OpenGLDebugger.h"

#include <sstream>
#include <list>
#include <set>
#include <glad/glad.h>

#include "Logger/Logger.h"

#ifdef __linux
#include <signal.h>
#include <GL/glx.h>
#endif

namespace SCION_RENDERING
{
struct Setting
{
	bool breakOnError{ false };
	bool breakOnWarning{ false };
	OpenGLDebuggerSeverity severity;
	std::set<unsigned int> ignoreList;
} static inline s_settings;

static inline void glDebugCallback( GLenum p_source, GLenum p_type, unsigned int p_id, GLenum p_severity,
									GLsizei p_length, const char* p_message, const void* p_userParam );

void OpenGLDebugger::init()
{
	static bool s_init{ false };
	if ( s_init )
	{
		return;
	}

	s_settings.severity = OpenGLDebuggerSeverity::Notification;

	// Default ignored Ids
	push( 131169 );
	push( 131185 );
	push( 131204 );
	push( 31218 );

	SCION_LOG( "Setting up OpenGL debug callback (requires OpenGL version > 4.3.)" );

	{
#ifdef _WIN32
		if ( !wglGetCurrentContext() )
#endif // _WIN32
#ifdef __linux
			if ( !glXGetCurrentContext() )
#endif // __linux
			{
				SCION_WARN( "There is no valid OpenGL context bound." );
				s_init = false;
				return;
			}
	}

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
		::glEnable( GL_DEBUG_OUTPUT );
		::glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		::glDebugMessageCallback( glDebugCallback, NULL );
		::glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE );
	}

	s_init = true;
}

void OpenGLDebugger::init( const std::vector<unsigned int>& p_ignoreList )
{
	s_settings.ignoreList.clear();

	for ( const auto& id : p_ignoreList )
	{
		push( id );
	}

	init();
}

void OpenGLDebugger::push( unsigned int p_errorId )
{
	s_settings.ignoreList.insert( p_errorId );
}

void OpenGLDebugger::pop( unsigned int p_errorId )
{
	if ( s_settings.ignoreList.contains( p_errorId ) )
	{
		s_settings.ignoreList.erase( p_errorId );
	}
}

void OpenGLDebugger::clear()
{
	s_settings.ignoreList.clear();
}

std::vector<unsigned int> OpenGLDebugger::getIgnoreList()
{
	std::vector<unsigned int> list;

	for ( const auto& id : s_settings.ignoreList )
	{
		list.push_back( id );
	}

	return list;
}

void OpenGLDebugger::breakOnError( bool b )
{
	s_settings.breakOnError = b;
}

void OpenGLDebugger::breakOnWarning( bool b )
{
	s_settings.breakOnWarning = b;
}

void OpenGLDebugger::setSeverityLevel( OpenGLDebuggerSeverity s )
{
	switch ( s )
	{
	case OpenGLDebuggerSeverity::Disable: s_settings.severity = OpenGLDebuggerSeverity::Disable; break;
	case OpenGLDebuggerSeverity::Notification: s_settings.severity = OpenGLDebuggerSeverity::Notification; break;
	case OpenGLDebuggerSeverity::Low: s_settings.severity = OpenGLDebuggerSeverity::Low; break;
	case OpenGLDebuggerSeverity::Medium: s_settings.severity = OpenGLDebuggerSeverity::Medium; break;
	case OpenGLDebuggerSeverity::High: s_settings.severity = OpenGLDebuggerSeverity::High; break;
	default: break;
	}
}

void glDebugCallback( GLenum p_source, GLenum p_type, unsigned int p_id, GLenum p_severity, GLsizei p_length,
					  const char* p_message, const void* p_userParam )
{
	OpenGLDebugger::init();

	(void)p_length;
	(void)p_userParam;

	if ( s_settings.severity == OpenGLDebuggerSeverity::Disable )
	{
		return;
	}

	if ( s_settings.ignoreList.contains( p_id ) )
	{
		return;
	}

	const char* errorSeverity = nullptr;
	OpenGLDebuggerSeverity severity = OpenGLDebuggerSeverity::Notification;
	switch ( p_severity )
	{
	case GL_DEBUG_SEVERITY_HIGH:
		errorSeverity = "High";
		severity = OpenGLDebuggerSeverity::High;
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		errorSeverity = "Medium";
		severity = OpenGLDebuggerSeverity::Medium;
		break;
	case GL_DEBUG_SEVERITY_LOW:
		errorSeverity = "Low";
		severity = OpenGLDebuggerSeverity::Low;
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		errorSeverity = "Notification";
		severity = OpenGLDebuggerSeverity::Notification;
		break;
	default: errorSeverity = "Unknown"; break;
	}

	if ( severity < s_settings.severity )
	{
		return;
	}

	const char* msgSource = nullptr;
	switch ( p_source )
	{
	case GL_DEBUG_SOURCE_API: msgSource = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: msgSource = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: msgSource = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: msgSource = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: msgSource = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: msgSource = "Other"; break;
	default: msgSource = "NULL"; break;
	}

	const char* errorType = nullptr;
	switch ( p_type )
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
	default: errorType = "Unknown"; break;
	}

	std::stringstream ss;
	// clang-format off
	ss
	<< "---> OpenGL Debug Callback <---"
	<< "\n\t\tSource..: " << msgSource
	<< "\n\t\tType....: " << errorType
	<< "\n\t\tSeverity: " << errorSeverity
	<< "\n\t\tId......: " << p_id
	<< "\n\t\tMessage.: " << p_message;

	switch ( p_severity )
	{
	case GL_DEBUG_SEVERITY_HIGH:
		SCION_LOGGER::Logger::GetInstance().Error( ss.str() );
if(s_settings.breakOnError)
{
			#ifdef _WIN32
						__debugbreak();
			#elif __linux
						raise( SIGTRAP );
			#endif
}
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
	case GL_DEBUG_SEVERITY_LOW:
		SCION_LOGGER::Logger::GetInstance().Warn( ss.str() );

if(s_settings.breakOnWarning)
{
			#ifdef _WIN32
						__debugbreak();
			#elif __linux
						raise( SIGTRAP );
			#endif
}
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		SCION_LOGGER::Logger::GetInstance().Log( ss.str() );
		break;
	default:
		break;
	}
	// clang-format on
}
} // namespace SCION_RENDERING
