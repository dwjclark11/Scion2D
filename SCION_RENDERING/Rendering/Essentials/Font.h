#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "Vertex.h"

namespace SCION_RENDERING {
	struct FontGlyph
	{
		Vertex min, max;
	};

	class Font
	{
	private:
		GLuint m_FontAtlasID;
		int m_Width, m_Height;
		float m_FontSize;
		void* m_pData;
	
	public:
		Font(GLuint fontAtlasID, int width, int height, float fontSize, void* data);
		~Font();

		FontGlyph GetGlyph(char c, glm::vec2& pos);
		void GetNextCharPos(char c, glm::vec2& pos);
		inline const GLuint GetFontAtlasID() const { return m_FontAtlasID; }
		inline const float GetFontSize() const { return m_FontSize; }
	};
}