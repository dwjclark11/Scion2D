#pragma once
#include <glad/glad.h>
#include <string>

namespace SCION_RENDERING
{
class Texture
{
  public:
	enum class TextureType
	{
		PIXEL = 0,
		BLENDED,
		FRAMEBUFFER,
		NONE
	};

  private:
	GLuint m_TextureID;
	int m_Width, m_Height;
	std::string m_sPath;
	TextureType m_eType;

  public:
	Texture();
	Texture( GLuint id, int width, int height, TextureType type = TextureType::PIXEL,
			 const std::string& texturePath = "" );
	~Texture() = default;

	inline const int GetWidth() const { return m_Width; }
	inline const int GetHeight() const { return m_Height; }
	inline const GLuint GetID() const { return m_TextureID; }

	void Bind();
	void Unbind();
};
} // namespace SCION_RENDERING
