#include "Rendering/Essentials/FontLoader.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#undef STB_TRUETYPE_IMPLEMENTATION

#include <cmath>
#include <cstddef>
#include <fstream>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <Logger/Logger.h>

namespace Scion::Rendering
{
namespace
{

[[nodiscard]] std::unique_ptr<Font> CreateFont( const unsigned char* fontData, float fontSize, int width, int height,
												std::string filename )
{
	// Validate inputs
	if ( !fontData || !std::isfinite( fontSize ) || fontSize <= 0.f || width <= 0 || height <= 0 )
	{
		SCION_ERROR( "Invalid font data, size, or atlas dimensions [{}].", filename );
		return nullptr;
	}

	const auto atlasWidth = static_cast<std::size_t>( width );
	const auto atlasHeight = static_cast<std::size_t>( height );

	if ( atlasWidth > std::numeric_limits<std::size_t>::max() / atlasHeight )
	{
		SCION_ERROR( "Font atlas dimensions overflow [{}].", filename );
		return nullptr;
	}

	// Validate maximum supported texture dimensions
	GLint maxTextureSize{};
	glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxTextureSize );

	if ( maxTextureSize <= 0 || width > maxTextureSize || height > maxTextureSize )
	{
		SCION_ERROR( "Font atlas {}x{} exceeds supported OpenGL texture dimensions [{}].", width, height, filename );
		return nullptr;
	}

	// Initialize font information
	const int fontOffset = stbtt_GetFontOffsetForIndex( fontData, 0 );

	stbtt_fontinfo fontInfo{};

	if ( fontOffset < 0 || !stbtt_InitFont( &fontInfo, fontData, fontOffset ) )
	{
		SCION_ERROR( "Failed to initialize font info [{}].", filename );
		return nullptr;
	}

	// Generate font bitmap and glyph data
	std::vector<unsigned char> bitmap( atlasWidth * atlasHeight );
	Font::BakedCharacters bakedCharacters{};

	const int bakeResult = stbtt_BakeFontBitmap( fontData,
												 fontOffset,
												 fontSize,
												 bitmap.data(),
												 width,
												 height,
												 Font::FirstCharacter,
												 static_cast<int>( Font::CharacterCount ),
												 bakedCharacters.data() );

	if ( bakeResult <= 0 )
	{
		SCION_ERROR( "Failed to bake {} glyphs into {}x{} atlas [{}].", Font::CharacterCount, width, height, filename );
		return nullptr;
	}

	// Calculate font ascent
	int ascent{};
	int descent{};
	int lineGap{};

	stbtt_GetFontVMetrics( &fontInfo, &ascent, &descent, &lineGap );

	const float scale = stbtt_ScaleForPixelHeight( &fontInfo, fontSize );
	const float fontAscent = static_cast<float>( ascent ) * scale;

	// Preserve existing OpenGL state
	GLint previousTexture{};
	GLint previousAlignment{};

	glGetIntegerv( GL_TEXTURE_BINDING_2D, &previousTexture );
	glGetIntegerv( GL_UNPACK_ALIGNMENT, &previousAlignment );

	// Create font texture
	GLuint fontId{};
	glGenTextures( 1, &fontId );

	if ( fontId == 0 )
	{
		SCION_ERROR( "Failed to generate font texture [{}].", filename );
		return nullptr;
	}

	glBindTexture( GL_TEXTURE_2D, fontId );

	// The bitmap contains one byte per pixel
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.data() );

	// Verify the texture storage was allocated
	GLint uploadedWidth{};
	GLint uploadedHeight{};

	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &uploadedWidth );

	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &uploadedHeight );

	if ( uploadedWidth != width || uploadedHeight != height )
	{
		SCION_ERROR( "Failed to allocate font texture [{}].", filename );

		glPixelStorei( GL_UNPACK_ALIGNMENT, previousAlignment );
		glBindTexture( GL_TEXTURE_2D, static_cast<GLuint>( previousTexture ) );
		glDeleteTextures( 1, &fontId );

		return nullptr;
	}

	// Configure texture sampling
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// Restore previous OpenGL state
	glPixelStorei( GL_UNPACK_ALIGNMENT, previousAlignment );
	glBindTexture( GL_TEXTURE_2D, static_cast<GLuint>( previousTexture ) );

	// Transfer texture ownership to Font
	try
	{
		return std::make_unique<Font>(
			fontId, width, height, fontSize, std::move( bakedCharacters ), fontAscent, std::move( filename ) );
	}
	catch ( ... )
	{
		glDeleteTextures( 1, &fontId );
		throw;
	}
}

} // namespace

std::unique_ptr<Font> FontLoader::Create( const std::string& fontPath, float fontSize, int width, int height )
{
	std::ifstream fontStream{ fontPath, std::ios::binary | std::ios::ate };
	if ( !fontStream )
	{
		SCION_ERROR( "Failed to open font [{}].", fontPath );
		return nullptr;
	}

	const auto end = fontStream.tellg();
	if ( end <= 0 || end > static_cast<std::streamoff>( std::numeric_limits<std::streamsize>::max() ) )
	{
		SCION_ERROR( "Invalid font file size [{}].", fontPath );
		return nullptr;
	}

	const auto length = static_cast<std::streamsize>( end );
	std::vector<unsigned char> buffer( static_cast<std::size_t>( length ) );
	fontStream.seekg( 0, std::ios::beg );
	if ( !fontStream.read( reinterpret_cast<char*>( buffer.data() ), length ) )
	{
		SCION_ERROR( "Failed to read font file [{}].", fontPath );
		return nullptr;
	}

	return CreateFont( buffer.data(), fontSize, width, height, fontPath );
}

std::unique_ptr<Font> FontLoader::CreateFromMemory( std::span<const unsigned char> fontData, float fontSize, int width,
													int height )
{
	if ( fontData.empty() )
	{
		SCION_ERROR( "Cannot load font from an empty buffer." );
		return nullptr;
	}

	return CreateFont( fontData.data(), fontSize, width, height, {} );
}

std::unique_ptr<Font> FontLoader::CreateFromMemory( const unsigned char* fontData, float fontSize, int width,
													int height )
{
	return CreateFont( fontData, fontSize, width, height, {} );
}

} // namespace Scion::Rendering
