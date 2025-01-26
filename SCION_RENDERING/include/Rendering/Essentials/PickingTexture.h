#pragma once
#include <glad/glad.h>
#include <cstdint>

namespace SCION_RENDERING
{
class PickingTexture
{
  public:
	PickingTexture();
	PickingTexture( int width, int height );
	~PickingTexture();

	void Bind();
	void Unbind();
	void Resize( int width, int height );
	void CheckResize();

	uint32_t ReadPixel( int x, int y ) const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

  private:
	bool Init( int width, int height );
	void CleanUp();

  private:
	GLuint m_TextureID;
	GLuint m_FBO;
	GLuint m_DepthTexture;
	int m_Width;
	int m_Height;
	bool m_bShouldResize;
};
} // namespace SCION_RENDERING
