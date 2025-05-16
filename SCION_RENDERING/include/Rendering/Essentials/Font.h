#pragma once
#include "Vertex.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <map>

namespace SCION_RENDERING
{
struct FontGlyph
{
	Vertex min;
	Vertex max;
};

struct PaddingInfo
{
	float paddingX{ 0.f };
	float paddingY{ 0.f };
};

class Font
{
  public:
	Font( GLuint fontAtlasID, int width, int height, float fontSize, void* data, float fontAscent = 0.f );
	~Font();

	FontGlyph GetGlyph( char c, glm::vec2& pos );
	void GetNextCharPos( char c, glm::vec2& pos );
	const PaddingInfo& GetPaddingInfoForChar( char c ) const;

	inline const GLuint GetFontAtlasID() const { return m_FontAtlasID; }
	inline const float GetFontSize() const { return m_FontSize; }
	const PaddingInfo& AveragePaddingInfo() const { return m_AveragePadding; }

  private:
	/* Opengl texture Id */
	GLuint m_FontAtlasID;
	/* The width of the font atlas */
	int m_Width;
	/* The height of the font atlas */
	int m_Height;
	/* The desired max size of the font. */
	float m_FontSize;
	/* Distance from the baseline to highest point in a glyph, in pixels. */
	float m_FontAscent;
	/* The underlying stbtt_bakedchar data. */
	void* m_pData;
	/* The average padding of all characters. */
	PaddingInfo m_AveragePadding;
	/* Map of each character and their padding info. */
	std::map<char, PaddingInfo> m_mapPaddingInfo;
};
} // namespace SCION_RENDERING
