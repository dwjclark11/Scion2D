#include "Renderer.h"
#include "../Essentials/Shader.h"
#include "Camera2D.h"

namespace SCION_RENDERING {

	Renderer::Renderer()
		: m_pLineBatch{nullptr}, m_pSpriteBatch{nullptr}
	{
		m_pLineBatch = std::make_unique<LineBatchRenderer>();
		m_pSpriteBatch = std::make_unique<SpriteBatchRenderer>();
	}

	void Renderer::SetClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
	{
	}

	void Renderer::ClearBuffers(bool color, bool depth, bool stencil)
	{
	}

	void Renderer::SetBlendCapability(BlendingFactors sFactor, BlendingFactors dFactor)
	{
	}

	void Renderer::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height)
	{
	}

	void Renderer::DrawLine(const Line& line)
	{
		m_Lines.push_back(line);
	}

	void Renderer::DrawLine(const glm::vec2& p1, const glm::vec2& p2, const Color& color, float lineWidth)
	{
		m_Lines.push_back(Line{.p1 = p1, .p2 = p2, .lineWidth = lineWidth, .color = color});
	}

	void Renderer::DrawRect(const Rect& rect)
	{
	}

	void Renderer::DrawRect(const glm::vec2& position, float width, float height, const Color& color)
	{
	}

	void Renderer::DrawFilledRect(const Rect& rect)
	{
	}

	void Renderer::DrawCircle(const Circle& circle)
	{
	}

	void Renderer::DrawCircle(const glm::vec2& position, float radius, const Color& color, float thickness)
	{
	}

	void Renderer::DrawLines(Shader& shader, Camera2D& camera)
	{
		auto cam_mat = camera.GetCameraMatrix();
		shader.Enable();
		shader.SetUniformMat4("projection", cam_mat);

		m_pLineBatch->Begin();

		for (const auto& line : m_Lines)
		{
			m_pLineBatch->AddLine(line);
		}
		m_pLineBatch->End();
		m_pLineBatch->Render();
		shader.Disable();
	}

	void Renderer::DrawRects()
	{
	}

	void Renderer::DrawFilledRects()
	{
	}

	void Renderer::DrawCircles()
	{
	}

	void Renderer::ClearPrimitives()
	{
	}

}