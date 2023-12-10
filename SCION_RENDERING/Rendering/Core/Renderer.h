#pragma once
#include <glad/glad.h>
#include "../Essentials/Primitives.h"
#include "BatchRenderer.h"
#include "LineBatchRenderer.h"

namespace SCION_RENDERING {
	class Renderer
	{
	public:
		enum class GLCapability : GLuint
		{
			BLEND		= GL_BLEND,
			DEPTH_TEST	= GL_DEPTH_TEST,
			LINE_SMOOTH = GL_LINE_SMOOTH,
		};

		enum class BlendingFactors : GLuint
		{
			ONE					= GL_ONE,
			SRC_ALPHA			= GL_SRC_ALPHA,
			ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
		};

	private:
		std::vector<Line> m_Lines;
		std::vector<Rect> m_Rects;
		std::vector<Circle> m_Circles;

		std::unique_ptr<LineBatchRenderer> m_pLineBatch;
		//std::unique_ptr<CircleBatchRenderer> m_pCircleBatch;
		std::unique_ptr<SpriteBatchRenderer> m_pSpriteBatch;

	public:
		Renderer();
		~Renderer() = default;
		
		// OpenGL Render function wrappers
		void SetClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
		void ClearBuffers(bool color = true, bool depth = true, bool stencil = false);

		void SetBlendCapability(BlendingFactors sFactor, BlendingFactors dFactor);
		void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height);

		// Primitive Draw calls
		void DrawLine(const Line& line);
		void DrawLine(const glm::vec2& p1, const glm::vec2& p2, const Color& color, float lineWidth = 1.f);

		void DrawRect(const Rect& rect);
		void DrawRect(const glm::vec2& position, float width, float height, const Color& color);
		void DrawFilledRect(const Rect& rect);

		void DrawCircle(const Circle& circle);
		void DrawCircle(const glm::vec2& position, float radius, const Color& color, float thickness = 1.f);


		void DrawLines(class Shader& shader, class Camera2D& camera);
		void DrawRects();
		void DrawFilledRects();
		void DrawCircles();

		void ClearPrimitives();
	};
}