#pragma once
#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace SCION_RENDERING
{
class Shader
{
  private:
	GLuint m_ShaderProgramID;
	std::string m_sVertexPath;
	std::string m_sFragmentPath;

	std::unordered_map<std::string, GLuint> m_UniformLocationMap;

  private:
	GLuint GetUniformLocation( const std::string& uniformName );

  public:
	Shader();
	Shader( GLuint program, const std::string vertexPath, const std::string& fragmentPath );
	~Shader();

	void SetUniformInt( const std::string& name, int value );
	void SetUniformFloat( const std::string& name, float value );

	void SetUniformVec2( const std::string& name, float x, float y );
	void SetUniformVec2( const std::string& name, const glm::vec2& value );

	void SetUniformVec3( const std::string& name, float x, float y, float z );
	void SetUniformVec3( const std::string& name, const glm::vec3& value );

	void SetUniformVec4( const std::string& name, float x, float y, float z, float w );
	void SetUniformVec4( const std::string& name, const glm::vec3& value );

	void SetUniformMat2( const std::string& name, const glm::mat2& mat );
	void SetUniformMat3( const std::string& name, const glm::mat3& mat );
	void SetUniformMat4( const std::string& name, const glm::mat4& mat );

	void Enable() const;
	void Disable() const;

	inline const GLuint ShaderProgramID() const { return m_ShaderProgramID; }
};
} // namespace SCION_RENDERING
