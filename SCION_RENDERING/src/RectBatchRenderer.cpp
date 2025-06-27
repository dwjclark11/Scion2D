#include "Rendering/Core/RectBatchRenderer.h"
#include "Rendering/Essentials/Primitives.h"

namespace SCION_RENDERING
{

void RectBatchRenderer::GenerateBatches()
{
	std::vector<Vertex> vertices;
	vertices.resize( (m_Glyphs.size() > MAX_SPRITES ? MAX_SPRITES : m_Glyphs.size()) * 4 );

	for ( const auto& shape : m_Glyphs )
	{
		if ( m_CurrentObject == 0 )
		{
			m_Batches.push_back(
				std::make_shared<RectBatch>( RectBatch{ .numIndices = NUM_SPRITE_INDICES, .offset = 0 } ) );
		}
		else
		{
			m_Batches.back()->numIndices += NUM_SPRITE_INDICES;
		}

		vertices[ m_CurrentVertex++ ] = shape->topLeft;
		vertices[ m_CurrentVertex++ ] = shape->topRight;
		vertices[ m_CurrentVertex++ ] = shape->bottomRight;
		vertices[ m_CurrentVertex++ ] = shape->bottomLeft;

		m_CurrentObject++;
		m_Offset += NUM_SPRITE_INDICES;

		// If the number of objects are equal to max sprites,
		// Flush early
		if ( m_CurrentObject == MAX_SPRITES )
		{
			Flush( vertices );
		}
	}

	// Buffer remaining data
	if ( !vertices.empty() && !m_Batches.empty() )
	{
		glBindBuffer( GL_ARRAY_BUFFER, GetVBO() );
		// Orphan the buffer
		glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), nullptr, GL_DYNAMIC_DRAW );
		// Upload the data
		glBufferSubData( GL_ARRAY_BUFFER, 0, vertices.size() * sizeof( Vertex ), vertices.data() );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}
}

void RectBatchRenderer::Initialize()
{
	SetVertexAttribute( 0, 2, GL_FLOAT, sizeof( Vertex ), (void*)offsetof( Vertex, position ) );
	SetVertexAttribute( 1, 4, GL_UNSIGNED_BYTE, sizeof( Vertex ), (void*)offsetof( Vertex, color ), GL_TRUE );
}

RectBatchRenderer::RectBatchRenderer()
	: Batcher( true )
{
	Initialize();
}

void RectBatchRenderer::End()
{
	// If the glyphs are empty, there is nothing to render
	if ( m_Glyphs.empty() )
		return;

	GenerateBatches();
}

void RectBatchRenderer::Render()
{
	EnableVAO();

	for ( const auto& batch : m_Batches )
	{
		glDrawElements( GL_TRIANGLES, batch->numIndices, GL_UNSIGNED_INT, (void*)( sizeof( GLuint ) * batch->offset ) );
	}

	DisableVAO();
}

void RectBatchRenderer::AddRect( const glm::vec4& destRect, int layer, const Color& color, glm::mat4 model )
{
	std::shared_ptr<RectGlyph> newGlyph = std::make_shared<RectGlyph>();

	newGlyph->topLeft.color = color;
	newGlyph->topLeft.position = model * glm::vec4{ destRect.x, destRect.y + destRect.w, 0.f, 1.f };

	newGlyph->bottomLeft.color = color;
	newGlyph->bottomLeft.position = model * glm::vec4{ destRect.x, destRect.y, 0.f, 1.f };

	newGlyph->bottomRight.color = color;
	newGlyph->bottomRight.position = model * glm::vec4{ destRect.x + destRect.z, destRect.y, 0.f, 1.f };

	newGlyph->topRight.color = color;
	newGlyph->topRight.position = model * glm::vec4{ destRect.x + destRect.z, destRect.y + destRect.w, 0.f, 1.f };

	m_Glyphs.push_back( std::move( newGlyph ) );
}

void RectBatchRenderer::AddRect( const Rect& rect, glm::mat4 model )
{
	std::shared_ptr<RectGlyph> newGlyph = std::make_shared<RectGlyph>();

	newGlyph->topLeft.color = rect.color;
	newGlyph->topLeft.position = model * glm::vec4{ rect.position.x, rect.position.y + rect.height, 0.f, 1.f };

	newGlyph->bottomLeft.color = rect.color;
	newGlyph->bottomLeft.position = model * glm::vec4{ rect.position.x, rect.position.y, 0.f, 1.f };

	newGlyph->bottomRight.color = rect.color;
	newGlyph->bottomRight.position = model * glm::vec4{ rect.position.x + rect.width, rect.position.y, 0.f, 1.f };

	newGlyph->topRight.color = rect.color;
	newGlyph->topRight.position =
		model * glm::vec4{ rect.position.x + rect.width, rect.position.y + rect.height, 0.f, 1.f };

	m_Glyphs.push_back( std::move( newGlyph ) );
}
void RectBatchRenderer::AddIsoRect( const Rect& rect, glm::mat4 model )
{
	std::shared_ptr<RectGlyph> newGlyph = std::make_shared<RectGlyph>();

	newGlyph->topLeft.color = rect.color;
	newGlyph->topLeft.position = model * glm::vec4{ rect.position.x, rect.position.y, 0.f, 1.f };

	newGlyph->bottomLeft.color = rect.color;
	newGlyph->bottomLeft.position =
		model * glm::vec4{ rect.position.x - rect.width / 2, rect.position.y + rect.height / 2, 0.f, 1.f };

	newGlyph->bottomRight.color = rect.color;
	newGlyph->bottomRight.position = model * glm::vec4{ rect.position.x, rect.position.y + rect.height, 0.f, 1.f };

	newGlyph->topRight.color = rect.color;
	newGlyph->topRight.position =
		model * glm::vec4{ rect.position.x + rect.width / 2, rect.position.y + rect.height / 2, 0.f, 1.f };

	m_Glyphs.push_back( std::move( newGlyph ) );
}
} // namespace SCION_RENDERING
