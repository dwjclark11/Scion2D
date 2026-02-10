#include "Rendering/Core/PickingBatchRenderer.h"
#include <algorithm>

namespace Scion::Rendering
{

void PickingBatchRenderer::Initialize()
{
	SetVertexAttribute( 0, 2, GL_FLOAT, sizeof( PickingVertex ), (void*)offsetof( PickingVertex, position ) );
	SetVertexAttribute( 1, 2, GL_FLOAT, sizeof( PickingVertex ), (void*)offsetof( PickingVertex, uvs ) );
	SetVertexIAttribute( 2, 1, GL_UNSIGNED_INT, sizeof( PickingVertex ), (void*)offsetof( PickingVertex, uid ) );
}

void PickingBatchRenderer::GenerateBatches()
{
	std::vector<PickingVertex> vertices;
	vertices.resize( m_Glyphs.size() * NUM_SPRITE_VERTICES );

	int currentVertex{ 0 }, currentSprite{ 0 };
	GLuint offset{ 0 }, prevTextureID{ 0 };

	for ( const auto& sprite : m_Glyphs )
	{
		if ( currentSprite == 0 )
			m_Batches.emplace_back( std::make_unique<Batch>(
				Batch{ .numIndices = NUM_SPRITE_INDICES, .offset = offset, .textureID = sprite->textureID } ) );
		else if ( sprite->textureID != prevTextureID )
			m_Batches.emplace_back( std::make_unique<Batch>(
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

	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( PickingVertex ), nullptr, GL_DYNAMIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, vertices.size() * sizeof( PickingVertex ), vertices.data() );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

PickingBatchRenderer::PickingBatchRenderer()
	: Batcher( true )
{
	Initialize();
}

void PickingBatchRenderer::End()
{
	if ( m_Glyphs.empty() )
		return;

	// Sort the sprites by their layer
	std::ranges::sort( m_Glyphs, [ & ]( const auto& a, const auto& b ) { return a->layer < b->layer; } );

	GenerateBatches();
}

void PickingBatchRenderer::Render()
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

void PickingBatchRenderer::AddSprite( const glm::vec4& spriteRect, const glm::vec4 uvRect, GLuint textureID, int layer,
									  uint32_t id, const Color& color, glm::mat4 model )
{
	// clang-format off
	m_Glyphs.emplace_back(
		std::make_unique<PickingGlyph>(
			PickingGlyph{
				.topLeft = PickingVertex{
					.position = model * glm::vec4{ spriteRect.x, spriteRect.y + spriteRect.w, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x, uvRect.y + uvRect.w },
					.color = color,
					.uid = id
				},
				.bottomLeft = PickingVertex{
					.position = model * glm::vec4{ spriteRect.x, spriteRect.y, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x, uvRect.y },
					.color = color,
					.uid = id
				},
				.topRight = PickingVertex{
					.position = model * glm::vec4{ spriteRect.x + spriteRect.z, spriteRect.y + spriteRect.w, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x + uvRect.z, uvRect.y + uvRect.w },
					.color = color,
					.uid = id
				},
				.bottomRight = PickingVertex{
					.position = model * glm::vec4{ spriteRect.x + spriteRect.z, spriteRect.y, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x + uvRect.z, uvRect.y },
					.color = color,
					.uid = id
				},
				.layer = layer,
				.textureID = textureID
			}
		)
	);
	// clang-format on
}
} // namespace Scion::Rendering
