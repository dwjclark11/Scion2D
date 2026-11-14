#pragma once

#include "Vertex.h"

#include <array>
#include <cstddef>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb_truetype.h>

namespace Scion::Rendering
{

struct FontGlyph
{
	Vertex min{};
	Vertex max{};
};

struct PaddingInfo
{
	float paddingX{ 0.f };
	float paddingY{ 0.f };
};

class Font
{
  public:
	static constexpr int FirstCharacter{ 32 };
	static constexpr std::size_t CharacterCount{ 96 };
	using BakedCharacters = std::array<stbtt_bakedchar, CharacterCount>;

	// Takes ownership of the OpenGL texture. The baked glyph data is stored by value.
	Font( GLuint fontAtlasID, int width, int height, float fontSize, BakedCharacters bakedCharacters,
		  float fontAscent = 0.f, std::string filename = {} );
	~Font();

	Font( const Font& ) = delete;
	Font& operator=( const Font& ) = delete;
	Font( Font&& ) = delete;
	Font& operator=( Font&& ) = delete;

	[[nodiscard]] FontGlyph GetGlyph( char c, glm::vec2& pos ) const;
	void GetNextCharPos( char c, glm::vec2& pos ) const;
	[[nodiscard]] const PaddingInfo& GetPaddingInfoForChar( char c ) const noexcept;

	[[nodiscard]] GLuint GetFontAtlasID() const noexcept { return m_FontAtlasID; }
	[[nodiscard]] float GetFontSize() const noexcept { return m_FontSize; }
	[[nodiscard]] const PaddingInfo& AveragePaddingInfo() const noexcept { return m_AveragePadding; }
	[[nodiscard]] const std::string& GetFilename() const noexcept { return m_sFilename; }

  private:
	GLuint m_FontAtlasID{};
	int m_Width{};
	int m_Height{};
	float m_FontSize{};
	float m_FontAscent{};
	BakedCharacters m_BakedCharacters{};
	PaddingInfo m_AveragePadding{};
	std::array<PaddingInfo, CharacterCount> m_PaddingInfo{};
	std::string m_sFilename{};
};

} // namespace Scion::Rendering
