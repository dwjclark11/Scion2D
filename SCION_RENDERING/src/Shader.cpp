#include "Rendering/Essentials/Shader.h"
#include <iostream>
#include <Logger/Logger.h>
namespace SCION_RENDERING
{

GLuint Shader::GetUniformLocation(const std::string& uniformName)
{
	auto uniformItr = m_UniformLocationMap.find(uniformName);
	if (uniformItr != m_UniformLocationMap.end())
		return uniformItr->second;

	GLuint location = glGetUniformLocation(m_ShaderProgramID, uniformName.c_str());

	if (location == GL_INVALID_INDEX)
	{
		SCION_ERROR("Uniform [{0}] not found in the shader!", uniformName);
		return -1;
	}

	m_UniformLocationMap.emplace(uniformName, location);

	return location;
}

Shader::Shader()
	: Shader(0, "", "")
{
}

Shader::Shader(GLuint program, const std::string vertexPath, const std::string& fragmentPath)
	: m_ShaderProgramID{program}
	, m_sVertexPath{vertexPath}
	, m_sFragmentPath{fragmentPath}
{
}

Shader::~Shader()
{
	if (m_ShaderProgramID > 0)
		glDeleteProgram(m_ShaderProgramID);
}

void Shader::SetUniformInt(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniformMat4(const std::string& name, glm::mat4& mat)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[ 0 ][ 0 ]);
}

void Shader::Enable()
{
	glUseProgram(m_ShaderProgramID);
}

void Shader::Disable()
{
	glUseProgram(0);
}

} // namespace SCION_RENDERING