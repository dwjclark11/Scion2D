#include "Rendering/Core/Renderer.h"
#include "Rendering/Essentials/Shader.h"
#include "Rendering/Core/Camera2D.h"
#include "Rendering/Buffers/Framebuffer.h"
#include "Rendering/Core/ShapeBatchRenderer.h"
#include "Rendering/Core/BatchRenderer.h"
#include "Rendering/Core/TextBatchRenderer.h"

#include "Logger/Logger.h"
#include <sol/sol.hpp>

namespace Scion::Rendering
{

Renderer::Renderer()
	: m_Text{}
	, m_pShapeBatch{ std::make_unique<ShapeRenderer>() }
	, m_pSpriteBatch{ std::make_unique<SpriteBatchRenderer>() }
	, m_pTextBatch{ std::make_unique<TextBatchRenderer>() }
{
}

Renderer::~Renderer() = default;

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
	m_pShapeBatch->AddLine( line.p1, line.p2, line.lineWidth, line.color );
}

void Renderer::DrawLine( const glm::vec2& p1, const glm::vec2& p2, const Color& color, float lineWidth )
{
	m_pShapeBatch->AddLine( p1, p2, lineWidth, color );
}

void Renderer::DrawRect( const Rect& rect )
{
	m_pShapeBatch->AddRectOutline( rect.position, rect.width, rect.height, 1.f, rect.color );
}

void Renderer::DrawRect( const glm::vec2& position, float width, float height, const Color& color )
{
	m_pShapeBatch->AddRectOutline( position, width, height, 1.f, color );
}

void Renderer::DrawFilledRect( const Rect& rect )
{
	m_pShapeBatch->AddRect( rect.position, rect.width, rect.height, rect.color );
}

void Renderer::DrawCircle( const Circle& circle )
{
	// The old circle shader drew the circle inside a quad of size "radius", so radius behaved like a diameter.
	// That is kept here so existing scripts look the same.
	const float radius = circle.radius * 0.5f;
	const float thickness = std::clamp( circle.lineThickness, 0.f, 1.f );
	if ( radius <= 0.f || thickness <= 0.f )
		return;

	glm::mat4 model{ 1.f };
	model[ 3 ][ 0 ] = circle.position.x + radius;
	model[ 3 ][ 1 ] = circle.position.y + radius;

	const int segments = ShapeRenderer::CircleSegments( radius );

	// Thickness is a fraction of the radius, 1 is a filled disc
	if ( thickness >= 1.f )
		m_pShapeBatch->AddCircle( radius, segments, model, circle.color );
	else
		m_pShapeBatch->AddRing( radius, radius * ( 1.f - thickness ), segments, model, circle.color );
}

void Renderer::DrawCircle( const glm::vec2& position, float radius, const Color& color, float thickness )
{
	DrawCircle( Circle{ .position = position, .lineThickness = thickness, .radius = radius, .color = color } );
}

// Replaces DrawLines, DrawFilledRects and DrawCircles
void Renderer::DrawShapes( Shader& shader, Camera2D& camera )
{
	if ( m_pShapeBatch->Empty() )
		return;

	// The shader has to be enabled before End, End can draw full windows
	shader.Enable();
	shader.SetUniformMat4( "uProjection", camera.GetCameraMatrix() );

	m_pShapeBatch->End();
	m_pShapeBatch->Render();
	shader.Disable();
}

void Renderer::ClearPrimitives()
{
	m_Text.clear();
	m_pShapeBatch->Begin();
}

void Renderer::DrawText2D( const Text& text )
{
	m_Text.push_back( text );
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

void Renderer::BindTextureUnit( GLuint unit, GLuint textureID )
{
	glBindTextureUnit( unit, textureID );
}

void Renderer::CreateFBO( const std::string& name, int width, int height, bool bUseRBO )
{
	if ( m_mapFramebuffers.contains( name ) )
	{
		SCION_ERROR( "Failed to create frame buffer [{}]. Already exists in renderer.", name );
		return;
	}

	auto [ it, bSuccess ] = m_mapFramebuffers.emplace( name, std::make_unique<Framebuffer>( width, height, bUseRBO ) );
	if ( !bSuccess )
	{
		SCION_ERROR( "Failed to create frame buffer. Unknown error." );
	}
}

void Renderer::BindFBO( const std::string& name )
{
	// Deativate all FBOs
	for ( auto& [ _, pFBO ] : m_mapFramebuffers )
		pFBO->Unbind();

	if ( name.empty() )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	auto fboItr = m_mapFramebuffers.find( name );
	if ( fboItr == m_mapFramebuffers.end() )
	{
		SCION_ERROR( "Failed to bind Framebuffer [{}]. Does not exist.", name );
		return;
	}

	fboItr->second->Bind();
}

void Renderer::UnbindFBO()
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void Renderer::DeleteFBO( const std::string& name )
{
	auto fboItr = m_mapFramebuffers.find( name );
	if ( fboItr == m_mapFramebuffers.end() )
	{
		SCION_ERROR( "Failed to delete Framebuffer [{}]. Does not exist.", name );
		return;
	}

	m_mapFramebuffers.erase( fboItr );
}

bool Renderer::HasFBO( const std::string& name ) const
{
	return m_mapFramebuffers.find( name ) != m_mapFramebuffers.end();
}

GLuint Renderer::GetFBTextureID( const std::string& name )
{
	auto fboItr = m_mapFramebuffers.find( name );
	if ( fboItr == m_mapFramebuffers.end() )
	{
		SCION_ERROR( "Failed to get FB texture for Framebuffer [{}]. Does not exist.", name );
		return 0;
	}

	return fboItr->second->GetTextureID();
}

GLuint Renderer::GetActiveFBTextureID() const
{
	for ( const auto& [ _, pFBO ] : m_mapFramebuffers )
	{
		if ( pFBO->IsActive() )
		{
			return pFBO->GetTextureID();
		}
	}

	return 0;
}

Framebuffer* Renderer::GetFramebuffer( const std::string& name )
{
	auto fboItr = m_mapFramebuffers.find( name );
	if ( fboItr == m_mapFramebuffers.end() )
	{
		SCION_ERROR( "Failed to get FB texture for Framebuffer [{}]. Does not exist.", name );
		return 0;
	}

	return fboItr->second.get();
}

void Renderer::DrawScreen( GLuint textureID )
{
	glBindVertexArray( m_ScreenVAO );
	glBindTextureUnit( 0, textureID );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

void Renderer::ClearScreen( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, bool color, bool depth )
{
	SetClearColor( red, green, blue, alpha );
	ClearBuffers( color, depth, false );
}

void Renderer::OnViewportSizeChange( float width, float height )
{
	for ( auto& [ _, fb ] : m_mapFramebuffers )
	{
		fb->Resize( static_cast<int>( width ), static_cast<int>( height ) );
		fb->CheckResize();
	}
}

void Renderer::CreateLuaBind( sol::state& lua, Renderer& renderer )
{
	// clang-format off

	// Primitives Bind
	lua.new_usertype<Line>( "Line",
							sol::call_constructor,
							sol::factories( []( const glm::vec2& p1, const glm::vec2& p2, const Color& color ) {
								return Line{ .p1 = p1, .p2 = p2, .color = color };
							} ),
							"p1",
							&Line::p1,
							"p2",
							&Line::p2,
							"color",
							&Line::color );

	lua.new_usertype<Rect>(
		"Rect",
		sol::call_constructor,
		sol::factories( []( const glm::vec2& position, float width, float height, const Color& color ) {
			return Rect{ .position = position, .width = width, .height = height, .color = color };
		} ),
		"position",
		&Rect::position,
		"width",
		&Rect::width,
		"height",
		&Rect::height,
		"color",
		&Rect::color );

	lua.new_usertype<Circle>(
		"Circle",
		sol::call_constructor,
		sol::factories( []( const glm::vec2& position, float lineThickness, float radius, const Color& color ) {
			return Circle{ .position = position, .lineThickness = lineThickness, .radius = radius, .color = color };
		} ),
		"position",
		&Circle::position,
		"lineThickness",
		&Circle::lineThickness,
		"radius",
		&Circle::radius,
		"color",
		&Circle::color );

	lua.new_usertype<Renderer>(
		"Renderer",
		sol::no_constructor,
		"get", [&renderer]()->Renderer& { return renderer; },
		"createFBO", &Renderer::CreateFBO,
		"bindFBO", &Renderer::BindFBO,
		"unbindFBO", &Renderer::UnbindFBO,
		"deleteFBO", &Renderer::DeleteFBO,
		"hasFBO", &Renderer::HasFBO,
		"bindTextureUnit", &Renderer::BindTextureUnit,
		"getFboTextureID", &Renderer::GetFBTextureID,
		"drawScreenToTexture", &Renderer::DrawScreen,
		"clearScreen", &Renderer::ClearScreen,
		"drawRect", sol::overload(
			sol::resolve<void(const Rect&)>(&Renderer::DrawRect),
			sol::resolve<void(const glm::vec2&, float, float, const Color& color)>(&Renderer::DrawRect)
		),
		"drawLine", sol::overload(
			sol::resolve<void(const Line&)>(&Renderer::DrawLine),
			sol::resolve<void(const glm::vec2&, const glm::vec2&, const Color&, float)>(&Renderer::DrawLine)
		),
		"drawCircle", sol::overload(
			sol::resolve<void(const Circle&)>(&Renderer::DrawCircle),
			sol::resolve<void(const glm::vec2&, float, const Color&, float)>(&Renderer::DrawCircle)
		),
		"drawFilledRect", &Renderer::DrawFilledRect,
		"setBlendEnabled", [](Renderer& r, bool bEnabled) {
			r.SetCapability( Renderer::GLCapability::BLEND, bEnabled );
		},
		"setViewport", &Renderer::SetViewport
	);
}

} // namespace Scion::Rendering
