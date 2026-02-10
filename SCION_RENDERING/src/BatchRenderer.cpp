#include "Rendering/Core/BatchRenderer.h"
#include <algorithm>

namespace Scion::Rendering
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
	vertices.resize( (m_Glyphs.size() > MAX_SPRITES ? MAX_SPRITES : m_Glyphs.size()) * NUM_SPRITE_VERTICES );

	GLuint prevTextureID{ 0 };

	for ( const auto& sprite : m_Glyphs )
	{
		if ( m_CurrentObject == 0 )
		{
			m_Batches.emplace_back( std::make_unique<Batch>(
				Batch{ .numIndices = NUM_SPRITE_INDICES, .offset = m_Offset, .textureID = sprite->textureID } ) );
		}
		else if ( sprite->textureID != prevTextureID )
		{
			m_Batches.emplace_back( std::make_unique<Batch>(
				Batch{ .numIndices = NUM_SPRITE_INDICES, .offset = m_Offset, .textureID = sprite->textureID } ) );
		}
		else
		{
			m_Batches.back()->numIndices += NUM_SPRITE_INDICES;
		}

		vertices[ m_CurrentVertex++ ] = sprite->topLeft;
		vertices[ m_CurrentVertex++ ] = sprite->topRight;
		vertices[ m_CurrentVertex++ ] = sprite->bottomRight;
		vertices[ m_CurrentVertex++ ] = sprite->bottomLeft;

		prevTextureID = sprite->textureID;
		m_Offset += NUM_SPRITE_INDICES;
		m_CurrentObject++;

		if (m_CurrentObject == MAX_SPRITES)
		{
			Flush( vertices );
		}
	}

	// Buffer remaining data
	if ( !vertices.empty() && !m_Batches.empty() )
	{
		glBindBuffer( GL_ARRAY_BUFFER, GetVBO() );

		glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), nullptr, GL_DYNAMIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, vertices.size() * sizeof( Vertex ), vertices.data() );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}
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

	// clang-format off
	std::sort
	(
		m_Glyphs.begin(), m_Glyphs.end(),
		[ & ]( const auto& a, const auto& b )
		{
			return a->layer < b->layer;
		}
	);

	// clang-format on

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
	// clang-format off
	m_Glyphs.emplace_back(
		std::make_unique<SpriteGlyph>(
			SpriteGlyph{
				.topLeft = Vertex{
					.position = model * glm::vec4{ spriteRect.x, spriteRect.y + spriteRect.w, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x, uvRect.y + uvRect.w },
					.color = color
				},
				.bottomLeft = Vertex{
					.position = model * glm::vec4{ spriteRect.x, spriteRect.y, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x, uvRect.y },
					.color = color
				},
				.topRight = Vertex{
					.position = model * glm::vec4{ spriteRect.x + spriteRect.z, spriteRect.y + spriteRect.w, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x + uvRect.z, uvRect.y + uvRect.w },
					.color = color
				},
				.bottomRight = Vertex{
					.position = model * glm::vec4{ spriteRect.x + spriteRect.z, spriteRect.y, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x + uvRect.z, uvRect.y },
					.color = color
				},
				.layer = layer,
				.textureID = textureID
			}
		)
	);
	// clang-format off
}

void SpriteBatchRenderer::AddSpriteIso( const glm::vec4& spriteRect, const glm::vec4 uvRect, GLuint textureID,
										int cellX, int cellY, int layer, glm::mat4 model, const Color& color )
{
	// clang-format off
	m_Glyphs.emplace_back(
		std::make_unique<SpriteGlyph>(
			SpriteGlyph{
				.topLeft = Vertex{
					.position = model * glm::vec4{ spriteRect.x, spriteRect.y + spriteRect.w, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x, uvRect.y + uvRect.w },
					.color = color
				},
				.bottomLeft = Vertex{
					.position = model * glm::vec4{ spriteRect.x, spriteRect.y, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x, uvRect.y },
					.color = color
				},
				.topRight = Vertex{
					.position = model * glm::vec4{ spriteRect.x + spriteRect.z, spriteRect.y + spriteRect.w, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x + uvRect.z, uvRect.y + uvRect.w },
					.color = color
				},
				.bottomRight = Vertex{
					.position = model * glm::vec4{ spriteRect.x + spriteRect.z, spriteRect.y, 0.f, 1.f },
					.uvs = glm::vec2{ uvRect.x + uvRect.z, uvRect.y },
					.color = color
				},
				.layer = cellY + cellX + static_cast<int>( layer * spriteRect.w ),
				.textureID = textureID
			}
		)
	);
	// clang-format on
}

} // namespace Scion::Rendering
