#include "Rendering/Core/ShapeBatchRenderer.h"

#include <algorithm>
#include <cmath>

namespace Scion::Rendering
{
constexpr float PI = 3.1415926f;
constexpr float TWOPI = PI * 2.f;

constexpr GLsizeiptr MAX_VERTICES = 10000 * sizeof( ShapeVertex );
constexpr GLsizeiptr MAX_INDICES = 10000 * sizeof( GLuint );
constexpr GLsizeiptr MAX_INDICES_PER_FLUSH = 10000;

ShapeRenderer::ShapeRenderer()
	: m_VAO{ 0 }
	, m_VBO{ 0 }
	, m_EBO{ 0 }
	, m_Vertices{}
	, m_Indices{}
{
	Initialize();
}

ShapeRenderer::~ShapeRenderer()
{
	glDeleteBuffers( 1, &m_VBO );
	glDeleteBuffers( 1, &m_EBO );
	glDeleteVertexArrays( 1, &m_VAO );
}

glm::vec2 ShapeRenderer::TransformPoint( const glm::vec2& p, const glm::mat4& model )
{
	glm::vec4 result = model * glm::vec4( p, 0.f, 1.f );
	return glm::vec2( result );
}

int ShapeRenderer::CircleSegments( float radius )
{
	return std::clamp( static_cast<int>( radius * 2.f ), 12, 64 );
}

void ShapeRenderer::PushQuad( const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3,
							  const Color& color, int layer, bool bIsLine )
{
	ShapeGlyph glyph{};
	glyph.layer = layer;
	glyph.bIsLine = bIsLine;
	glyph.vertexOffset = static_cast<uint32_t>( m_Vertices.size() );
	glyph.indexOffset = static_cast<uint32_t>( m_Indices.size() );

	m_Vertices.emplace_back( ShapeVertex{ p0, color } );
	m_Vertices.emplace_back( ShapeVertex{ p1, color } );
	m_Vertices.emplace_back( ShapeVertex{ p2, color } );
	m_Vertices.emplace_back( ShapeVertex{ p3, color } );

	const GLuint base = glyph.vertexOffset;
	m_Indices.insert( m_Indices.end(), { base + 0, base + 1, base + 2, base + 2, base + 3, base + 0 } );

	glyph.vertexCount = 4;
	glyph.indexCount = 6;

	m_ShapeGlyphs.emplace_back( std::move( glyph ) );
}

void ShapeRenderer::AddRect( const glm::vec2& position, float width, float height, const glm::mat4& model,
							 const Color& color, int layer )
{
	PushQuad( TransformPoint( position, model ),
			  TransformPoint( { position.x + width, position.y }, model ),
			  TransformPoint( { position.x + width, position.y + height }, model ),
			  TransformPoint( { position.x, position.y + height }, model ),
			  color,
			  layer );
}

void ShapeRenderer::AddRect( const glm::vec2& position, float width, float height, const Color& color, int layer )
{
	PushQuad( position,
			  { position.x + width, position.y },
			  { position.x + width, position.y + height },
			  { position.x, position.y + height },
			  color,
			  layer );
}

void ShapeRenderer::AddQuad( const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3,
							 const glm::mat4& model, const Color& color, int layer )
{
	PushQuad( TransformPoint( p0, model ),
			  TransformPoint( p1, model ),
			  TransformPoint( p2, model ),
			  TransformPoint( p3, model ),
			  color,
			  layer );
}

void ShapeRenderer::AddLine( const glm::vec2& p1, const glm::vec2& p2, float width, const Color& color, int layer )
{
	const glm::vec2 delta = p2 - p1;
	const float length = std::sqrt( delta.x * delta.x + delta.y * delta.y );
	if ( length < 0.0001f )
		return;

	const glm::vec2 normal = glm::vec2{ -delta.y, delta.x } * ( ( width * 0.5f ) / length );

	PushQuad( p1 + normal, p2 + normal, p2 - normal, p1 - normal, color, layer, true );
}

void ShapeRenderer::AddRectOutline( const glm::vec2& position, float width, float height, float thickness,
									const Color& color, int layer )
{
	const float half = thickness * 0.5f;

	// Top and bottom span the full width including the corners
	AddRect( { position.x - half, position.y - half }, width + thickness, thickness, color, layer );
	AddRect( { position.x - half, position.y + height - half }, width + thickness, thickness, color, layer );

	// Left and right fit between them
	AddRect( { position.x - half, position.y + half }, thickness, height - thickness, color, layer );
	AddRect( { position.x + width - half, position.y + half }, thickness, height - thickness, color, layer );
}

void ShapeRenderer::AddTriangle( const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::mat4& model,
								 const Color& color, int layer )
{
	ShapeGlyph glyph{};
	glyph.layer = layer;
	glyph.vertexOffset = static_cast<uint32_t>( m_Vertices.size() );
	glyph.indexOffset = static_cast<uint32_t>( m_Indices.size() );

	m_Vertices.emplace_back( ShapeVertex{ TransformPoint( p1, model ), color } );
	m_Vertices.emplace_back( ShapeVertex{ TransformPoint( p2, model ), color } );
	m_Vertices.emplace_back( ShapeVertex{ TransformPoint( p3, model ), color } );

	const GLuint base = glyph.vertexOffset;
	m_Indices.insert( m_Indices.end(), { base, base + 1, base + 2 } );

	glyph.vertexCount = 3;
	glyph.indexCount = 3;

	m_ShapeGlyphs.emplace_back( std::move( glyph ) );
}

void ShapeRenderer::AddRing( float outerRadius, float innerRadius, int segments, const glm::mat4& model,
							 const Color& color, int layer )
{
	if ( segments < 3 )
		return;

	ShapeGlyph glyph{};
	glyph.layer = layer;
	glyph.vertexOffset = static_cast<uint32_t>( m_Vertices.size() );
	glyph.indexOffset = static_cast<uint32_t>( m_Indices.size() );

	const GLuint base = glyph.vertexOffset;

	for ( int i = 0; i < segments; ++i )
	{
		const float angle = static_cast<float>( i ) / segments * TWOPI;
		const glm::vec2 dir{ std::cos( angle ), std::sin( angle ) };

		m_Vertices.emplace_back( ShapeVertex{ TransformPoint( dir * outerRadius, model ), color } );
		m_Vertices.emplace_back( ShapeVertex{ TransformPoint( dir * innerRadius, model ), color } );
	}

	for ( int i = 0; i < segments; ++i )
	{
		const GLuint o0 = base + i * 2;
		const GLuint i0 = o0 + 1;
		const GLuint o1 = base + ( ( i + 1 ) % segments ) * 2;
		const GLuint i1 = o1 + 1;

		m_Indices.insert( m_Indices.end(), { o0, i0, o1, o1, i0, i1 } );
	}

	glyph.vertexCount = static_cast<uint32_t>( segments * 2 );
	glyph.indexCount = static_cast<uint32_t>( segments * 6 );

	m_ShapeGlyphs.emplace_back( std::move( glyph ) );
}

void ShapeRenderer::AddRectangle( const glm::vec2& size, const glm::mat4& model, const Color& color, int layer )
{
	ShapeGlyph glyph{};
	glyph.layer = layer;
	glyph.vertexOffset = static_cast<uint32_t>( m_Vertices.size() );
	glyph.indexOffset = static_cast<uint32_t>( m_Indices.size() );

	glm::vec2 half = size * 0.5f;

	glm::vec2 local[ 4 ] = { { -half.x, -half.y }, { half.x, -half.y }, { half.x, half.y }, { -half.x, half.y } };

	for ( int i = 0; i < 4; ++i )
	{
		glm::vec2 world = TransformPoint( local[ i ], model );
		m_Vertices.emplace_back( ShapeVertex{ world, color } );
	}

	GLuint base = glyph.vertexOffset;

	m_Indices.insert( m_Indices.end(), { base + 0, base + 1, base + 2, base + 2, base + 3, base + 0 } );

	glyph.vertexCount = 4;
	glyph.indexCount = 6;

	m_ShapeGlyphs.emplace_back( std::move( glyph ) );
}

void ShapeRenderer::AddPolygon( const std::vector<glm::vec2>& points, const glm::mat4& model, const Color& color,
								int layer )
{
	if ( points.size() < 3 )
		return;

	ShapeGlyph glyph{};
	glyph.layer = layer;
	glyph.vertexOffset = static_cast<uint32_t>( m_Vertices.size() );
	glyph.indexOffset = static_cast<uint32_t>( m_Indices.size() );

	for ( const auto& p : points )
	{
		glm::vec2 world = TransformPoint( p, model );
		m_Vertices.emplace_back( ShapeVertex{ world, color } );
	}

	GLuint base = glyph.vertexOffset;

	for ( size_t i = 1; i < points.size() - 1; ++i )
	{
		m_Indices.push_back( base );
		m_Indices.push_back( base + i );
		m_Indices.push_back( base + i + 1 );
	}

	glyph.vertexCount = static_cast<uint32_t>( points.size() );
	glyph.indexCount = static_cast<uint32_t>( ( points.size() - 2 ) * 3 );

	m_ShapeGlyphs.emplace_back( std::move( glyph ) );
}

void ShapeRenderer::AddCircle( float radius, int segments, const glm::mat4& model, const Color& color, int layer )
{
	ShapeGlyph glyph{};
	glyph.layer = layer;
	glyph.vertexOffset = static_cast<uint32_t>( m_Vertices.size() );
	glyph.indexOffset = static_cast<uint32_t>( m_Indices.size() );

	GLuint base = glyph.vertexOffset;

	// Center
	m_Vertices.emplace_back( ShapeVertex{ TransformPoint( { 0.f, 0.f }, model ), color } );

	for ( int i = 0; i <= segments; ++i )
	{
		float angle = (float)i / segments * TWOPI;
		glm::vec2 local = { std::cos( angle ) * radius, std::sin( angle ) * radius };

		glm::vec2 world = TransformPoint( local, model );
		m_Vertices.emplace_back( ShapeVertex{ world, color } );
	}

	for ( int i = 1; i <= segments; ++i )
	{
		m_Indices.push_back( base );
		m_Indices.push_back( base + i );
		m_Indices.push_back( base + ( i % segments ) + 1 );
	}

	glyph.vertexCount = segments + 2;
	glyph.indexCount = segments * 3;

	m_ShapeGlyphs.emplace_back( std::move( glyph ) );
}

void ShapeRenderer::Begin()
{
	m_ShapeGlyphs.clear();
	m_Batches.clear();
	m_Vertices.clear();
	m_Indices.clear();
}

void ShapeRenderer::End()
{
	std::stable_sort(
		m_ShapeGlyphs.begin(), m_ShapeGlyphs.end(), []( const auto& a, const auto& b ) { return a.layer < b.layer; } );

	CreateBatches();
}

void ShapeRenderer::CreateBatches()
{
	if ( m_ShapeGlyphs.empty() )
		return;

	m_Batches.clear();

	// Working buffers for the current flush window only.
	// m_Vertices / m_Indices are still built by Add* functions,
	// but we re-gather them per-window here using the glyph metadata.
	std::vector<ShapeVertex> windowVertices;
	std::vector<GLuint> windowIndices;

	GLuint currentLayer = m_ShapeGlyphs[ 0 ].layer;
	GLuint batchOffset = 0; // index offset within the current window

	ShapeBatch batch{};
	batch.offset = 0;
	batch.layer = currentLayer;

	auto Flush = [ & ]() {
		if ( windowVertices.empty() || m_Batches.empty() )
			return;

		glNamedBufferSubData( m_VBO, 0, windowVertices.size() * sizeof( ShapeVertex ), windowVertices.data() );
		glNamedBufferSubData( m_EBO, 0, windowIndices.size() * sizeof( GLuint ), windowIndices.data() );

		glBindVertexArray( m_VAO );
		for ( const auto& b : m_Batches )
		{
			glDrawElements( GL_TRIANGLES, b.numIndices, GL_UNSIGNED_INT, (void*)( b.offset * sizeof( GLuint ) ) );
		}

		windowVertices.clear();
		windowIndices.clear();
		m_Batches.clear();
		batchOffset = 0;
	};

	for ( const auto& glyph : m_ShapeGlyphs )
	{
		if ( windowIndices.size() + glyph.indexCount > MAX_INDICES_PER_FLUSH )
		{
			// Close current batch and flush before this glyph
			batch.numIndices = static_cast<GLuint>( windowIndices.size() ) - batch.offset;
			if ( batch.numIndices > 0 )
				m_Batches.emplace_back( std::move( batch ) );

			Flush();

			batch = ShapeBatch{};
			batch.offset = 0;
			batch.layer = currentLayer;
		}

		if ( glyph.layer != currentLayer )
		{
			batch.numIndices = static_cast<GLuint>( windowIndices.size() ) - batch.offset;
			if ( batch.numIndices > 0 )
				m_Batches.push_back( std::move( batch ) );

			currentLayer = glyph.layer;
			batch = ShapeBatch{};
			batch.offset = static_cast<GLuint>( windowIndices.size() );
			batch.layer = currentLayer;
		}

		// Copy this glyph's verts into the window buffer.
		// Indices need rebasing to be relative to windowVertices, not m_Vertices.
		GLuint vertexBase = static_cast<GLuint>( windowVertices.size() );

		for ( GLuint i = 0; i < glyph.vertexCount; ++i )
			windowVertices.push_back( m_Vertices[ glyph.vertexOffset + i ] );

		for ( GLuint i = 0; i < glyph.indexCount; ++i )
		{
			// Rebase: original index was relative to m_Vertices base,
			// subtract glyph->vertexOffset then add window-local vertexBase
			GLuint rebased = m_Indices[ glyph.indexOffset + i ] - glyph.vertexOffset + vertexBase;
			windowIndices.push_back( rebased );
		}
	}

	// Close and upload final window
	batch.numIndices = static_cast<GLuint>( windowIndices.size() ) - batch.offset;
	if ( batch.numIndices > 0 )
		m_Batches.emplace_back( std::move( batch ) );

	if ( !windowVertices.empty() && !m_Batches.empty() )
	{
		glNamedBufferSubData( m_VBO, 0, windowVertices.size() * sizeof( ShapeVertex ), windowVertices.data() );
		glNamedBufferSubData( m_EBO, 0, windowIndices.size() * sizeof( GLuint ), windowIndices.data() );
	}
}

void ShapeRenderer::Render()
{
	glBindVertexArray( m_VAO );

	for ( const auto& batch : m_Batches )
	{
		glDrawElements( GL_TRIANGLES, batch.numIndices, GL_UNSIGNED_INT, (void*)( batch.offset * sizeof( GLuint ) ) );
	}
}

void ShapeRenderer::Initialize()
{
	glCreateVertexArrays( 1, &m_VAO );
	glCreateBuffers( 1, &m_VBO );
	glCreateBuffers( 1, &m_EBO );

	// GL_DYNAMIC_STORAGE_BIT allows glNamedBufferSubData updates later
	glNamedBufferStorage( m_VBO, MAX_VERTICES, nullptr, GL_DYNAMIC_STORAGE_BIT );
	glNamedBufferStorage( m_EBO, MAX_INDICES, nullptr, GL_DYNAMIC_STORAGE_BIT );

	// Attach EBO to VAO
	glVertexArrayElementBuffer( m_VAO, m_EBO );

	// Bind VBO to VAO at binding index 0 with stride
	glVertexArrayVertexBuffer( m_VAO, 0, m_VBO, 0, sizeof( ShapeVertex ) );

	// Position (vec2) - location 0
	glEnableVertexArrayAttrib( m_VAO, 0 );
	glVertexArrayAttribFormat( m_VAO, 0, 2, GL_FLOAT, GL_FALSE, offsetof( ShapeVertex, position ) );
	glVertexArrayAttribBinding( m_VAO, 0, 0 );

	// Color (4 unsigned bytes, normalized) - location 1
	glEnableVertexArrayAttrib( m_VAO, 1 );
	glVertexArrayAttribFormat( m_VAO, 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, offsetof( ShapeVertex, color ) );
	glVertexArrayAttribBinding( m_VAO, 1, 0 );
}

} // namespace Scion::Rendering
