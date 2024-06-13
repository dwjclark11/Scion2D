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
	std::string m_sVertexPath, m_sFragmentPath;

	std::unordered_map<std::string, GLuint> m_UniformLocationMap;

	GLuint GetUniformLocation( const std::string& uniformName );

  public:
	Shader();
	Shader( GLuint program, const std::string vertexPath, const std::string& fragmentPath );
	~Shader();

	void SetUniformInt( const std::string& name, int value );

	// TODO: Add more Setters as needed

	void SetUniformMat4( const std::string& name, glm::mat4& mat );

	// TODO: Add the getters for the uniforms

	void Enable();
	void Disable();

	inline const GLuint ShaderProgramID() const { return m_ShaderProgramID; }
};
} // namespace SCION_RENDERING