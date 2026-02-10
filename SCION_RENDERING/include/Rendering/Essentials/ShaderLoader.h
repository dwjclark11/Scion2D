#pragma once
#include "Shader.h"
#include <memory>

namespace Scion::Rendering
{
class ShaderLoader
{
  public:
	ShaderLoader() = delete;
	static std::shared_ptr<Shader> Create( const std::string& vertexShaderPath, const std::string& fragmentShaderPath );
	static std::shared_ptr<Shader> CreateFromMemory( const char* vertexShader, const char* fragmentShader );
	static bool Destroy( Shader* pShader );

  private:
	static GLuint CreateProgram( const std::string& vertexShader, const std::string& fragmentShader );
	static GLuint CompileShader( GLuint shaderType, const std::string& filepath );

	static GLuint CreateProgram( const char* vertexShader, const char* fragmentShader );
	static GLuint CompileShader( GLuint type, const char* shader );

	static bool CompileSuccess( GLuint shader );
	static bool IsProgramValid( GLuint program );
	static bool LinkShaders( GLuint program, GLuint vertexShader, GLuint fragmentShader );
};
} // namespace Scion::Rendering
