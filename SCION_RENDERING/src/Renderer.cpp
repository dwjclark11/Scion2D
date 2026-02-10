#include "Rendering/Core/Renderer.h"
#include "Rendering/Essentials/Shader.h"
#include "Rendering/Core/Camera2D.h"

namespace Scion::Rendering
{

Renderer::Renderer()
	: m_Lines{}
	, m_Rects{}
	, m_Circles{}
	, m_Text{}
	, m_pLineBatch{ std::make_unique<LineBatchRenderer>() }
	, m_pRectBatch{ std::make_unique<RectBatchRenderer>() }
	, m_pCircleBatch{ std::make_unique<CircleBatchRenderer>() }
	, m_pSpriteBatch{ std::make_unique<SpriteBatchRenderer>() }
	, m_pTextBatch{ std::make_unique<TextBatchRenderer>() }
{
}

void Renderer::SetClearColor( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
	glClearColor( red, green, blue, alpha );
}

void Renderer::ClearBuffers( bool color, bool depth, bool stencil )
{
	glClear( ( color ? GL_COLOR_BUFFER_BIT : 0 ) | ( depth ? GL_DEPTH_BUFFER_BIT : 0 ) |
			 ( stencil ? GL_STENCIL_BUFFER_BIT : 0 ) );
}

void Renderer::SetCapability( GLCapability capability, bool enable )
{
	if ( enable )
		glEnable( static_cast<GLenum>( capability ) );
	else
		glDisable( static_cast<GLenum>( capability ) );
}

bool Renderer::IsCapabilityEnabled( GLCapability capability ) const
{
	return glIsEnabled( static_cast<GLenum>( capability ) );
}

void Renderer::SetBlendCapability( BlendingFactors sFactor, BlendingFactors dFactor )
{
	glBlendFunc( static_cast<GLenum>( sFactor ), static_cast<GLenum>( dFactor ) );
}

void Renderer::SetViewport( GLint x, GLint y, GLsizei width, GLsizei height )
{
	glViewport( x, y, width, height );
}

void Renderer::SetLineWidth( GLfloat lineWidth )
{
	glLineWidth( lineWidth );
}

void Renderer::DrawLine( const Line& line )
{
	m_Lines.push_back( line );
}

void Renderer::DrawLine( const glm::vec2& p1, const glm::vec2& p2, const Color& color, float lineWidth )
{
	m_Lines.emplace_back( Line{ .p1 = p1, .p2 = p2, .lineWidth = lineWidth, .color = color } );
}

void Renderer::DrawRect( const Rect& rect )
{
	// Top
	DrawLine( Line{
		.p1 = rect.position, .p2 = glm::vec2{ rect.position.x + rect.width, rect.position.y }, .color = rect.color } );

	// Bottom
	DrawLine( Line{ .p1 = glm::vec2{ rect.position.x, rect.position.y + rect.height },
					.p2 = glm::vec2{ rect.position.x + rect.width, rect.position.y + rect.height },
					.color = rect.color } );
	// Left
	DrawLine( Line{
		.p1 = rect.position, .p2 = glm::vec2{ rect.position.x, rect.position.y + rect.height }, .color = rect.color } );

	// Right
	DrawLine( Line{ .p1 = glm::vec2{ rect.position.x + rect.width, rect.position.y },
					.p2 = glm::vec2{ rect.position.x + rect.width, rect.position.y + rect.height },
					.color = rect.color } );
}

void Renderer::DrawRect( const glm::vec2& position, float width, float height, const Color& color )
{
	DrawRect( Rect{ .position = position, .width = width, .height = height, .color = color } );
}

void Renderer::DrawFilledRect( const Rect& rect )
{
	m_Rects.push_back( rect );
}

void Renderer::DrawCircle( const Circle& circle )
{
	m_Circles.push_back( circle );
}

void Renderer::DrawCircle( const glm::vec2& position, float radius, const Color& color, float thickness )
{
	m_Circles.push_back( Circle{ .position = position, .lineThickness = thickness, .radius = radius, .color = color } );
}

void Renderer::DrawText2D( const Text& text )
{
	m_Text.push_back( text );
}

void Renderer::DrawLines( Shader& shader, Camera2D& camera )
{
	if ( m_Lines.empty() )
		return;

	auto cam_mat = camera.GetCameraMatrix();
	shader.Enable();
	shader.SetUniformMat4( "uProjection", cam_mat );

	m_pLineBatch->Begin();

	for ( const auto& line : m_Lines )
	{
		m_pLineBatch->AddLine( line );
	}

	m_pLineBatch->End();
	m_pLineBatch->Render();
	shader.Disable();
}

void Renderer::DrawFilledRects( Shader& shader, Camera2D& camera )
{
	if ( m_Rects.empty() )
		return;

	auto cam_mat = camera.GetCameraMatrix();
	shader.Enable();
	shader.SetUniformMat4( "uProjection", cam_mat );

	m_pRectBatch->Begin();

	for ( const auto& rect : m_Rects )
	{
		m_pRectBatch->AddRect( rect );
	}
	m_pRectBatch->End();
	m_pRectBatch->Render();
	shader.Disable();
}

void Renderer::DrawCircles( Shader& shader, Camera2D& camera )
{
	if ( m_Circles.empty() )
		return;

	auto cam_mat = camera.GetCameraMatrix();
	shader.Enable();
	shader.SetUniformMat4( "uProjection", cam_mat );

	m_pCircleBatch->Begin();

	for ( const auto& circle : m_Circles )
	{
		m_pCircleBatch->AddCircle( circle );
	}

	m_pCircleBatch->End();
	m_pCircleBatch->Render();
	shader.Disable();
}

void Renderer::DrawAllText( Shader& shader, Camera2D& camera )
{
	if ( m_Text.empty() )
		return;

	auto cam_mat = camera.GetCameraMatrix();
	shader.Enable();
	shader.SetUniformMat4( "uProjection", cam_mat );

	m_pTextBatch->Begin();

	for ( const auto& text : m_Text )
	{
		m_pTextBatch->AddText( text.textStr, text.pFont, text.position, 4, text.wrap, text.color );
	}

	m_pTextBatch->End();
	m_pTextBatch->Render();
	shader.Disable();
}

void Renderer::ClearPrimitives()
{
	m_Lines.clear();
	m_Rects.clear();
	m_Circles.clear();
	m_Text.clear();
}
} // namespace Scion::Rendering
