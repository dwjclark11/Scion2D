#include "Rendering/Essentials/PickingTexture.h"
#include "Logger/Logger.h"
#include <entt/entt.hpp>

namespace Scion::Rendering
{
PickingTexture::PickingTexture()
	: PickingTexture( 640, 480 )
{
}

PickingTexture::PickingTexture( int width, int height )
	: m_TextureID{ 0 }
	, m_FBO{ 0 }
	, m_DepthTexture{ 0 }
	, m_Width{ width }
	, m_Height{ height }
	, m_bShouldResize{ false }
{
	if (!Init(width, height))
	{
		SCION_ASSERT( false && "Failed to create picking texture." );
		SCION_ERROR( "Failed to create picking texture." );
		// Should this throw??
	}
}

PickingTexture::~PickingTexture()
{
	CleanUp();
}

void PickingTexture::Bind()
{
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_FBO );
}

void PickingTexture::Unbind()
{
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
}

void PickingTexture::Resize( int width, int height )
{
	m_Width = width;
	m_Height = height;
	m_bShouldResize = true;
}

void PickingTexture::CheckResize()
{
	if ( !m_bShouldResize )
		return;

	CleanUp();

	Init( m_Width, m_Height );
	m_bShouldResize = false;
}

uint32_t PickingTexture::ReadPixel( int x, int y ) const
{
	glBindFramebuffer( GL_READ_FRAMEBUFFER, m_FBO );
	glReadBuffer( GL_COLOR_ATTACHMENT0 );

	uint32_t pixelData{ static_cast<uint32_t>( entt::null ) };
	glReadPixels( x, m_Height - y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixelData );
	glReadBuffer( GL_NONE );
	glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );

	return pixelData;
}

bool PickingTexture::Init( int width, int height )
{
	// Generate the framebuffer
	glGenFramebuffers( 1, &m_FBO );
	glBindFramebuffer( GL_FRAMEBUFFER, m_FBO );

	// Create the texture
	glGenTextures( 1, &m_TextureID );
	glBindTexture( GL_TEXTURE_2D, m_TextureID );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0 );

	// Create the texture object for the depth buffer
	glEnable( GL_TEXTURE_2D );
	glGenTextures( 1, &m_DepthTexture );
	glBindTexture( GL_TEXTURE_2D, m_DepthTexture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0 );

	// Disable the reading
	glReadBuffer( GL_NONE );
	glDrawBuffer( GL_COLOR_ATTACHMENT0 );

	if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
	{
		std::string error = std::to_string( glGetError() );
		SCION_ERROR( "Failed to create a OpenGL frame buffer - {}", error );
		assert( false && "Failed to create an OpenGL frame buffer" );
		return false;
	}

	// unbind the texture and framebuffer
	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	return true;
}
void PickingTexture::CleanUp()
{
	if ( m_FBO > 0 )
	{
		glDeleteFramebuffers( 1, &m_FBO );
		m_FBO = 0;
	}

	if ( m_TextureID > 0 )
	{
		glDeleteTextures( 1, &m_TextureID );
		m_TextureID = 0;
	}

	if ( m_DepthTexture > 0 )
	{
		glDeleteTextures( 1, &m_DepthTexture );
		m_DepthTexture = 0;
	}
}
} // namespace Scion::Rendering
