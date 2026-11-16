#include "Rendering/Essentials/Shader.h"
#include <Logger/Logger.h>

#include <sol/sol.hpp>

namespace Scion::Rendering
{

GLuint Shader::GetUniformLocation( const std::string& uniformName )
{
	auto uniformItr = m_UniformLocationMap.find( uniformName );
	if ( uniformItr != m_UniformLocationMap.end() )
		return uniformItr->second;

	GLuint location = glGetUniformLocation( m_ShaderProgramID, uniformName.c_str() );

	if ( location == GL_INVALID_INDEX )
	{
		SCION_ERROR( "Uniform [{0}] not found in the shader!", uniformName );
		return 0;
	}

	m_UniformLocationMap.emplace( uniformName, location );

	return location;
}

Shader::Shader()
	: Shader( 0, "", "" )
{
}

Shader::Shader( GLuint program, const std::string vertexPath, const std::string& fragmentPath )
	: m_ShaderProgramID{ program }
	, m_sVertexPath{ vertexPath }
	, m_sFragmentPath{ fragmentPath }
	, m_bEngineShader{ false }
{
}

Shader::~Shader()
{
	if ( m_ShaderProgramID > 0 )
		glDeleteProgram( m_ShaderProgramID );
}

void Shader::SetUniformInt( const std::string& name, int value )
{
	glUniform1i( GetUniformLocation( name ), value );
}

void Shader::SetUniformFloat( const std::string& name, float value )
{
	glUniform1f( GetUniformLocation( name ), value );
}

void Shader::SetUniformVec2( const std::string& name, float x, float y )
{
	glUniform2f( GetUniformLocation( name ), x, y );
}

void Shader::SetUniformVec2( const std::string& name, const glm::vec2& value )
{
	glUniform2fv( GetUniformLocation( name ), 1, &value[ 0 ] );
}

void Shader::SetUniformVec3( const std::string& name, float x, float y, float z )
{
	glUniform3f( GetUniformLocation( name ), x, y, z );
}

void Shader::SetUniformVec3( const std::string& name, const glm::vec3& value )
{
	glUniform3fv( GetUniformLocation( name ), 1, &value[ 0 ] );
}

void Shader::SetUniformVec4( const std::string& name, float x, float y, float z, float w )
{
	glUniform4f( GetUniformLocation( name ), x, y, z, w );
}

void Shader::SetUniformVec4( const std::string& name, const glm::vec3& value )
{
	glUniform4fv( GetUniformLocation( name ), 1, &value[ 0 ] );
}

void Shader::SetUniformMat2( const std::string& name, const glm::mat2& mat )
{
	glUniformMatrix2fv( GetUniformLocation( name ), 1, GL_FALSE, &mat[ 0 ][ 0 ] );
}

void Shader::SetUniformMat3( const std::string& name, const glm::mat3& mat )
{
	glUniformMatrix3fv( GetUniformLocation( name ), 1, GL_FALSE, &mat[ 0 ][ 0 ] );
}

void Shader::SetUniformMat4( const std::string& name, const glm::mat4& mat )
{
	glUniformMatrix4fv( GetUniformLocation( name ), 1, GL_FALSE, &mat[ 0 ][ 0 ] );
}

void Shader::Enable() const
{
	glUseProgram( m_ShaderProgramID );
}

void Shader::Disable() const
{
	glUseProgram( 0 );
}

void Shader::CreateLuaBind( sol::state& lua )
{
	lua.new_usertype<Shader>(
		"Shader",
		sol::no_constructor,
		"enable",
		&Shader::Enable,
		"disable",
		&Shader::Disable,
		"setUniform",
		sol::overload( []( Shader& shader,
						   const std::string& sUniformName,
						   bool val ) { shader.SetUniformInt( sUniformName, val ); },
					   []( Shader& shader, const std::string& sUniformName, int val ) {
						   shader.SetUniformInt( sUniformName, val );
					   },
					   []( Shader& shader, const std::string& sUniformName, float val ) {
						   shader.SetUniformFloat( sUniformName, val );
					   },
					   []( Shader& shader, const std::string& sUniformName, float x, float y ) {
						   shader.SetUniformVec2( sUniformName, x, y );
					   },
					   []( Shader& shader, const std::string& sUniformName, const glm::vec2& val ) {
						   shader.SetUniformVec2( sUniformName, val );
					   },
					   []( Shader& shader, const std::string& sUniformName, float x, float y, float z ) {
						   shader.SetUniformVec3( sUniformName, x, y, z );
					   },
					   []( Shader& shader, const std::string& sUniformName, const glm::vec3& val ) {
						   shader.SetUniformVec3( sUniformName, val );
					   },
					   []( Shader& shader, const std::string& sUniformName, float x, float y, float z, float w ) {
						   shader.SetUniformVec4( sUniformName, x, y, z, w );
					   },
					   []( Shader& shader, const std::string& sUniformName, const glm::vec4& val ) {
						   shader.SetUniformVec4( sUniformName, val );
					   } ) );
}

} // namespace Scion::Rendering
