#include "Rendering/Essentials/ShaderLoader.h"
#include <iostream>
#include <fstream>
#include <Logger/Logger.h>
namespace SCION_RENDERING
{

GLuint ShaderLoader::CreateProgram( const std::string& vertexShader, const std::string& fragmentShader )
{
	const GLuint program = glCreateProgram();

	const GLuint vertex = CompileShader( GL_VERTEX_SHADER, vertexShader );
	const GLuint fragment = CompileShader( GL_FRAGMENT_SHADER, fragmentShader );

	if ( vertex == 0 || fragment == 0 )
		return 0;

	if ( !LinkShaders( program, vertex, fragment ) )
	{
		SCION_ERROR( "Failed to Link Shaders!" );
		return 0;
	}

	return program;
}

GLuint ShaderLoader::CompileShader( GLuint shaderType, const std::string& filepath )
{
	const GLuint shaderID = glCreateShader( shaderType );

	std::ifstream ifs( filepath );

	if ( ifs.fail() )
	{
		SCION_ERROR( "Shader Failed to open [{}]", filepath );
		return 0;
	}

	std::string contents{};
	std::string line;

	while ( std::getline( ifs, line ) )
	{
		contents += line + "\n";
	}

	ifs.close();

	const char* contentsPtr = contents.c_str();
	glShaderSource( shaderID, 1, &contentsPtr, nullptr );

	glCompileShader( shaderID );

	if ( !CompileSuccess( shaderID ) )
	{
		SCION_ERROR( "Failed to compile shader [{}]", filepath );
		return 0;
	}

	return shaderID;
}

GLuint ShaderLoader::CreateProgram( const char* vertexShader, const char* fragmentShader )
{
	const GLuint program = glCreateProgram();

	const GLuint vertex = CompileShader( GL_VERTEX_SHADER, vertexShader );
	const GLuint fragment = CompileShader( GL_FRAGMENT_SHADER, fragmentShader );

	if ( vertex == 0 || fragment == 0 )
		return 0;

	if ( !LinkShaders( program, vertex, fragment ) )
	{
		SCION_ERROR( "Failed to Link Shaders!" );
		return 0;
	}

	return program;
}

GLuint ShaderLoader::CompileShader( GLuint type, const char* shader )
{
	const GLuint id = glCreateShader( type );
	glShaderSource( id, 1, &shader, nullptr );

	glCompileShader( id );

	// Check to see if the shader compiled correctly
	if ( !CompileSuccess( id ) )
	{
		SCION_ERROR( "Failed to compile shader from memory!" );
		return 0;
	}

	return id;
}

bool ShaderLoader::CompileSuccess( GLuint shader )
{
	GLint status;

	glGetShaderiv( shader, GL_COMPILE_STATUS, &status );

	if ( status != GL_TRUE )
	{
		GLint maxLength;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

		std::string errorLog( maxLength, ' ' );

		glGetShaderInfoLog( shader, maxLength, &maxLength, errorLog.data() );

		SCION_ERROR( "Shader Compilation failed: {}", std::string{ errorLog } );

		glDeleteShader( shader );
		return false;
	}

	return true;
}

bool ShaderLoader::IsProgramValid( GLuint program )
{
	GLint status;

	glGetProgramiv( program, GL_LINK_STATUS, &status );

	if ( status != GL_TRUE )
	{
		GLint maxLength;
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

		std::string errorLog( maxLength, ' ' );

		glGetProgramInfoLog( program, maxLength, &maxLength, errorLog.data() );

		SCION_ERROR( "Shader's Program failed to link: {}", std::string{ errorLog } );
		return false;
	}

	return true;
}

bool ShaderLoader::LinkShaders( GLuint program, GLuint vertexShader, GLuint fragmentShader )
{
	glAttachShader( program, vertexShader );
	glAttachShader( program, fragmentShader );

	glLinkProgram( program );

	if ( !IsProgramValid( program ) )
	{
		glDeleteProgram( program );
		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );

		return false;
	}

	glDetachShader( program, vertexShader );
	glDetachShader( program, fragmentShader );

	glDeleteShader( vertexShader );
	glDeleteShader( fragmentShader );

	return true;
}

std::shared_ptr<Shader> ShaderLoader::Create( const std::string& vertexShaderPath,
											  const std::string& fragmentShaderPath )
{
	GLuint program = CreateProgram( vertexShaderPath, fragmentShaderPath );

	if ( program )
		return std::make_shared<Shader>( program, vertexShaderPath, fragmentShaderPath );

	return nullptr;
}

std::shared_ptr<Shader> ShaderLoader::CreateFromMemory( const char* vertexShader, const char* fragmentShader )
{
	GLuint program = CreateProgram( vertexShader, fragmentShader );

	// From memory holds onto the shader itself, not the path
	if ( program )
		return std::make_shared<Shader>( program, vertexShader, fragmentShader );

	return nullptr;
}

bool ShaderLoader::Destroy( Shader* pShader )
{
	if ( pShader->ShaderProgramID() <= 0 )
		return false;

	glDeleteShader( pShader->ShaderProgramID() );
	return true;
}
} // namespace SCION_RENDERING
