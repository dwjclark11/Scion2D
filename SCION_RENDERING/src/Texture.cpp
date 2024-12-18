#include "Rendering/Essentials/Texture.h"

namespace SCION_RENDERING
{

Texture::Texture()
	: Texture( 0, 0, 0, TextureType::NONE, "", false )
{
}

Texture::Texture( GLuint id, int width, int height, TextureType type, const std::string& texturePath, bool bIsTileset )
	: m_TextureID{ id }
	, m_Width{ width }
	, m_Height{ height }
	, m_eType{ type }
	, m_sPath{ texturePath }
	, m_bTileset{ bIsTileset }
	, m_bEditorTexture{ false }
{
}

void Texture::Bind()
{
	glBindTexture( GL_TEXTURE_2D, m_TextureID );
}

void Texture::Unbind()
{
	glBindTexture( GL_TEXTURE_2D, 0 );
}
} // namespace SCION_RENDERING
