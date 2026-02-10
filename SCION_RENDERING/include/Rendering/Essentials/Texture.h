#pragma once
#include <glad/glad.h>
#include <string>

namespace Scion::Rendering
{
class Texture
{
  public:
	enum class TextureType
	{
		PIXEL = 0,
		BLENDED,
		FRAMEBUFFER,
		ICON,
		NONE
	};

  public:
	Texture();
	Texture( GLuint id, int width, int height, TextureType type = TextureType::PIXEL,
			 const std::string& texturePath = "", bool bTileset = false );
	~Texture() = default;

	inline const int GetWidth() const { return m_Width; }
	inline const int GetHeight() const { return m_Height; }
	inline const GLuint GetID() const { return m_TextureID; }
	inline const bool IsTileset() const { return m_bTileset; }
	inline TextureType GetType() const { return m_eType; }
	inline void SetIsTileset( bool bIsTileset ) { m_bTileset = bIsTileset; }
	inline const std::string& GetPath() const { return m_sPath; }
	inline const bool IsEditorTexture() const { return m_bEditorTexture; }
	inline void SetIsEditorTexture( bool bIsEditorTexture ) { m_bEditorTexture = bIsEditorTexture; }

	void Bind();
	void Unbind();

	/*
	* @brief Deletes the underlying OpenGL Texture.
	* Only use this if texture is no longer needed.
	*/
	void Destroy();

  private:
	GLuint m_TextureID;
	int m_Width;
	int m_Height;
	std::string m_sPath;
	TextureType m_eType;
	bool m_bTileset;
	bool m_bEditorTexture;
};
} // namespace Scion::Rendering
