#include "Rendering/Buffers/Framebuffer.h"
#include <Logger/Logger.h>
#include "Rendering/Essentials/TextureLoader.h"

namespace Scion::Rendering
{

bool Framebuffer::Initialize()
{
	glCreateFramebuffers( 1, &m_FboID );
	glBindFramebuffer( GL_FRAMEBUFFER, m_FboID );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTexture->GetID(), 0 );

	if ( m_bUseRbo )
	{
		glCreateRenderbuffers( 1, &m_RboID );
		glBindRenderbuffer( GL_RENDERBUFFER, m_RboID );

		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, m_Width, m_Height );

		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RboID );
	}

	if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
	{
		SCION_ASSERT( false && "Failed to create an OpenGL Framebuffer!" );

		std::string error = std::to_string( glGetError() );
		SCION_ERROR( "Failed to create an OpenGL framebuffer: {}", error );
		return false;
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	return true;
}

void Framebuffer::CleanUp()
{
	glDeleteFramebuffers( 1, &m_FboID );
	if ( m_bUseRbo )
		glDeleteRenderbuffers( 1, &m_RboID );

	if ( m_pTexture )
	{
		auto textureID = m_pTexture->GetID();
		glDeleteTextures( 1, &textureID );
	}
}

Framebuffer::Framebuffer()
	: Framebuffer( 640, 480, false )
{
}

Framebuffer::Framebuffer( int width, int height, bool bUseRbo )
	: m_FboID{ 0 }
	, m_RboID{ 0 }
	, m_pTexture{ nullptr }
	, m_Width{ width }
	, m_Height{ height }
	, m_bShouldResize{ false }
	, m_bUseRbo{ bUseRbo }
{
	m_pTexture = std::move( TextureLoader::Create( Texture::TextureType::FRAMEBUFFER, width, height ) );

	if ( !m_pTexture || !Initialize() )
	{
		SCION_ASSERT( false && "Failed to create Framebuffer!" );
		SCION_ERROR( "Framebuffer creation failed" );
	}
}

Framebuffer::~Framebuffer()
{
	CleanUp();
}

void Framebuffer::Bind()
{
	glBindFramebuffer( GL_FRAMEBUFFER, m_FboID );
}

void Framebuffer::Unbind()
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void Framebuffer::Resize( int width, int height )
{
	m_Width = width;
	m_Height = height;
	m_bShouldResize = true;
}

void Framebuffer::CheckResize()
{
	if ( !m_bShouldResize )
		return;

	CleanUp();
	m_pTexture.reset();
	m_pTexture = std::move( TextureLoader::Create( Texture::TextureType::FRAMEBUFFER, m_Width, m_Height ) );

	SCION_ASSERT( m_pTexture && "New Texture cannot be nullptr!" );

	Initialize();
	m_bShouldResize = false;
}
} // namespace Scion::Rendering
