#include "Rendering/Core/BatchRenderer.h"
#include <algorithm>

namespace SCION_RENDERING
{

void SpriteBatchRenderer::Initialize()
{
	SetVertexAttribute( 0, 2, GL_FLOAT, sizeof( Vertex ), (void*)offsetof( Vertex, position ) );
	SetVertexAttribute( 1, 2, GL_FLOAT, sizeof( Vertex ), (void*)offsetof( Vertex, uvs ) );
	SetVertexAttribute( 2, 4, GL_UNSIGNED_BYTE, sizeof( Vertex ), (void*)offsetof( Vertex, color ), GL_TRUE );
}

void SpriteBatchRenderer::GenerateBatches()
{
	std::vector<Vertex> vertices;
	vertices.resize( m_Glyphs.size() * NUM_SPRITE_VERTICES );

	int currentVertex{ 0 }, currentSprite{ 0 };
	GLuint offset{ 0 }, prevTextureID{ 0 };

	for ( const auto& sprite : m_Glyphs )
	{
		if ( currentSprite == 0 )
			m_Batches.emplace_back( std::make_shared<Batch>(
				Batch{ .numIndices = NUM_SPRITE_INDICES, .offset = offset, .textureID = sprite->textureID } ) );
		else if ( sprite->textureID != prevTextureID )
			m_Batches.emplace_back( std::make_shared<Batch>(
				Batch{ .numIndices = NUM_SPRITE_INDICES, .offset = offset, .textureID = sprite->textureID } ) );
		else
			m_Batches.back()->numIndices += NUM_SPRITE_INDICES;

		vertices[ currentVertex++ ] = sprite->topLeft;
		vertices[ currentVertex++ ] = sprite->topRight;
		vertices[ currentVertex++ ] = sprite->bottomRight;
		vertices[ currentVertex++ ] = sprite->bottomLeft;

		prevTextureID = sprite->textureID;
		offset += NUM_SPRITE_INDICES;
		currentSprite++;
	}

	glBindBuffer( GL_ARRAY_BUFFER, GetVBO() );

	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), nullptr, GL_DYNAMIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, vertices.size() * sizeof( Vertex ), vertices.data() );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

SpriteBatchRenderer::SpriteBatchRenderer()
	: Batcher( true )
{
	Initialize();
}

void SpriteBatchRenderer::End()
{
	if ( m_Glyphs.empty() )
		return;

	// Sort the sprites by their layer
	std::sort(
		m_Glyphs.begin(), m_Glyphs.end(), [ & ]( const auto& a, const auto& b ) { return a->layer < b->layer; } );

	GenerateBatches();
}

void SpriteBatchRenderer::Render()
{
	if ( m_Batches.empty() )
		return;

	EnableVAO();

	for ( const auto& batch : m_Batches )
	{
		glBindTextureUnit( 0, batch->textureID );
		glDrawElements( GL_TRIANGLES, batch->numIndices, GL_UNSIGNED_INT, (void*)( sizeof( GLuint ) * batch->offset ) );
	}

	DisableVAO();
}

void SpriteBatchRenderer::AddSprite( const glm::vec4& spriteRect, const glm::vec4 uvRect, GLuint textureID, int layer,
									 glm::mat4 model, const Color& color )
{
	auto newSprite = std::make_shared<SpriteGlyph>( SpriteGlyph{
		.topLeft = Vertex{ .position = model * glm::vec4{ spriteRect.x, spriteRect.y + spriteRect.w, 0.f, 1.f },
						   .uvs = glm::vec2{ uvRect.x, uvRect.y + uvRect.w },
						   .color = color },
		.bottomLeft = Vertex{ .position = model * glm::vec4{ spriteRect.x, spriteRect.y, 0.f, 1.f },
							  .uvs = glm::vec2{ uvRect.x, uvRect.y },
							  .color = color },
		.topRight =
			Vertex{ .position = model * glm::vec4{ spriteRect.x + spriteRect.z, spriteRect.y + spriteRect.w, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x + uvRect.z, uvRect.y + uvRect.w },
					.color = color },
		.bottomRight = Vertex{ .position = model * glm::vec4{ spriteRect.x + spriteRect.z, spriteRect.y, 0.f, 1.f },
							   .uvs = glm::vec2{ uvRect.x + uvRect.z, uvRect.y },
							   .color = color },
		.layer = layer,
		.textureID = textureID } );

	m_Glyphs.push_back( std::move( newSprite ) );
}
} // namespace SCION_RENDERING