#include "Rendering/Essentials/Font.h"

#include <cmath>
#include <utility>

namespace Scion::Rendering
{

Font::Font( GLuint fontAtlasID, int width, int height, float fontSize, BakedCharacters bakedCharacters,
			float fontAscent, std::string filename )
	: m_FontAtlasID{ fontAtlasID }
	, m_Width{ width }
	, m_Height{ height }
	, m_FontSize{ fontSize }
	, m_FontAscent{ fontAscent }
	, m_BakedCharacters{ std::move( bakedCharacters ) }
	, m_sFilename{ std::move( filename ) }
{
	float totalPaddingX{ 0.f };
	float totalPaddingY{ 0.f };

	for ( std::size_t i = 0; i < CharacterCount; ++i )
	{
		// Keep each measurement independent of preceding glyph advances.
		float x{ 0.f };
		float y{ 0.f };
		stbtt_aligned_quad quad{};
		stbtt_GetBakedQuad( m_BakedCharacters.data(), m_Width, m_Height, static_cast<int>( i ), &x, &y, &quad, 1 );

		auto& padding = m_PaddingInfo[ i ];
		padding.paddingX = m_FontSize - ( quad.x1 - quad.x0 );
		padding.paddingY = m_FontSize - ( quad.y1 - quad.y0 );
		totalPaddingX += padding.paddingX;
		totalPaddingY += padding.paddingY;
	}

	const float count = static_cast<float>( CharacterCount );
	m_AveragePadding.paddingX = std::floor( totalPaddingX / count );
	m_AveragePadding.paddingY = std::floor( totalPaddingY / count );
}

Font::~Font()
{
	if ( m_FontAtlasID != 0 )
	{
		// The owning OpenGL context must still be current when Font is destroyed.
		glDeleteTextures( 1, &m_FontAtlasID );
	}
}

FontGlyph Font::GetGlyph( char c, glm::vec2& pos ) const
{
	FontGlyph glyph{};
	const auto codepoint = static_cast<unsigned char>( c );
	if ( codepoint < FirstCharacter || codepoint >= FirstCharacter + CharacterCount )
	{
		return glyph;
	}

	float y = pos.y + m_FontAscent;
	stbtt_aligned_quad quad{};
	stbtt_GetBakedQuad( m_BakedCharacters.data(),
						m_Width,
						m_Height,
						static_cast<int>( codepoint - FirstCharacter ),
						&pos.x,
						&y,
						&quad,
						1 );

	glyph.min = Vertex{ .position = glm::vec2{ quad.x0, quad.y0 }, .uvs = glm::vec2{ quad.s0, quad.t0 } };
	glyph.max = Vertex{ .position = glm::vec2{ quad.x1, quad.y1 }, .uvs = glm::vec2{ quad.s1, quad.t1 } };
	return glyph;
}

void Font::GetNextCharPos( char c, glm::vec2& pos ) const
{
	const auto codepoint = static_cast<unsigned char>( c );
	if ( codepoint < FirstCharacter || codepoint >= FirstCharacter + CharacterCount )
	{
		return;
	}

	stbtt_aligned_quad quad{};
	stbtt_GetBakedQuad( m_BakedCharacters.data(),
						m_Width,
						m_Height,
						static_cast<int>( codepoint - FirstCharacter ),
						&pos.x,
						&pos.y,
						&quad,
						1 );
}

const PaddingInfo& Font::GetPaddingInfoForChar( char c ) const noexcept
{
	const auto codepoint = static_cast<unsigned char>( c );
	if ( codepoint < FirstCharacter || codepoint >= FirstCharacter + CharacterCount )
	{
		return m_AveragePadding;
	}

	return m_PaddingInfo[ codepoint - FirstCharacter ];
}

} // namespace Scion::Rendering
