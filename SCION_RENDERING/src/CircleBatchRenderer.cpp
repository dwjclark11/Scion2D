#include "Rendering/Core/CircleBatchRenderer.h"
#include "Rendering/Essentials/Primitives.h"

namespace SCION_RENDERING
{

void CircleBatchRenderer::GenerateBatches()
{
	std::vector<CircleVertex> vertices;
	vertices.resize( m_Glyphs.size() * NUM_SPRITE_VERTICES );

	for ( const auto& circle : m_Glyphs )
	{
		if ( m_CurrentObject == 0 )
		{
			m_Batches.emplace_back(
				std::make_unique<RectBatch>( RectBatch{ .numIndices = NUM_SPRITE_INDICES, .offset = m_Offset } ) );
		}
		else
		{
			m_Batches.back()->numIndices += NUM_SPRITE_INDICES;
		}

		vertices[ m_CurrentVertex++ ] = circle->topLeft;
		vertices[ m_CurrentVertex++ ] = circle->topRight;
		vertices[ m_CurrentVertex++ ] = circle->bottomRight;
		vertices[ m_CurrentVertex++ ] = circle->bottomLeft;

		m_Offset += NUM_SPRITE_INDICES;
		m_CurrentObject++;

		if (m_CurrentObject == MAX_SPRITES)
		{
			Flush(vertices);
		}
	}

	if ( !vertices.empty() && !m_Batches.empty())
	{
		glBindBuffer( GL_ARRAY_BUFFER, GetVBO() );

		glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( CircleVertex ), nullptr, GL_DYNAMIC_DRAW );

		glBufferSubData( GL_ARRAY_BUFFER, 0, vertices.size() * sizeof( CircleVertex ), vertices.data() );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}
}

void CircleBatchRenderer::Initialize()
{
	SetVertexAttribute( 0, 2, GL_FLOAT, sizeof( CircleVertex ), (void*)offsetof( CircleVertex, position ) );
	SetVertexAttribute( 1, 2, GL_FLOAT, sizeof( CircleVertex ), (void*)offsetof( CircleVertex, uvs ) );
	SetVertexAttribute(
		2, 4, GL_UNSIGNED_BYTE, sizeof( CircleVertex ), (void*)offsetof( CircleVertex, color ), GL_TRUE );
	SetVertexAttribute( 3, 1, GL_FLOAT, sizeof( CircleVertex ), (void*)offsetof( CircleVertex, lineThickness ) );
}

CircleBatchRenderer::CircleBatchRenderer()
	: Batcher( true )
{
	Initialize();
}

void CircleBatchRenderer::End()
{
	// We want to check to see if there are any sprites to batch
	if ( m_Glyphs.empty() )
		return;

	// Now we can generate the batches
	GenerateBatches();
}

void CircleBatchRenderer::AddCircle( const glm::vec4& destRect, const Color& color, float thickness, glm::mat4 model )
{
	// clang-format off
	m_Glyphs.emplace_back(
		std::make_unique<CircleGlyph>(
			CircleGlyph{
				.topLeft = CircleVertex{
					.position = model * glm::vec4{ destRect.x, destRect.y + destRect.w, 0.f, 1.f },
					.uvs = glm::vec2{ 1.f, 1.f },
					.color = color,
					.lineThickness = thickness
				},
				.bottomLeft = CircleVertex{
					.position = model * glm::vec4{ destRect.x, destRect.y, 0.f, 1.f },
					.uvs = glm::vec2{ 1.f, -1.f },
					.color = color,
					.lineThickness = thickness
				},
				.topRight = CircleVertex{
					.position = model * glm::vec4{ destRect.x + destRect.z, destRect.y + destRect.w, 0.f, 1.f },
					.uvs = glm::vec2{ -1.f, 1.f },
					.color = color,
					.lineThickness = thickness
				},
				.bottomRight = CircleVertex{
					.position = model * glm::vec4{ destRect.x + destRect.z, destRect.y, 0.f, 1.f },
					.uvs = glm::vec2{ -1.f, -1.f },
					.color = color,
					.lineThickness = thickness
				},
			}
		)
	);
	// clang-format on
}

void CircleBatchRenderer::AddCircle( const Circle& circle )
{
	// clang-format off
	glm::mat4 model{ 1.f };
	m_Glyphs.emplace_back(
		std::make_unique<CircleGlyph>(
			CircleGlyph{
				.topLeft = CircleVertex{
					.position = model * glm::vec4{ circle.position.x, circle.position.y + circle.radius, 0.f, 1.f },
					.uvs = glm::vec2{ 1.f, 1.f },
					.color = circle.color,
					.lineThickness = circle.lineThickness
				},
				.bottomLeft = CircleVertex{
					.position = model * glm::vec4{ circle.position.x, circle.position.y, 0.f, 1.f },
					.uvs = glm::vec2{ 1.f, -1.f },
					.color = circle.color,
					.lineThickness = circle.lineThickness
				},
				.topRight = CircleVertex{
					.position = model * glm::vec4{ circle.position.x + circle.radius, circle.position.y + circle.radius, 0.f, 1.f },
					.uvs = glm::vec2{ -1.f, 1.f },
					.color = circle.color,
					.lineThickness = circle.lineThickness
				},
				.bottomRight = CircleVertex{
					.position = model * glm::vec4{ circle.position.x + circle.radius, circle.position.y, 0.f, 1.f },
					.uvs = glm::vec2{ -1.f, -1.f },
					.color = circle.color,
					.lineThickness = circle.lineThickness
				},
			}
		)
	);

	// clang-format on
}

void CircleBatchRenderer::Render()
{
	if ( m_Batches.empty() )
		return;

	EnableVAO();

	for ( const auto& batch : m_Batches )
	{
		glDrawElements( GL_TRIANGLES, batch->numIndices, GL_UNSIGNED_INT, (void*)( sizeof( GLuint ) * batch->offset ) );
	}

	DisableVAO();
}
} // namespace SCION_RENDERING
