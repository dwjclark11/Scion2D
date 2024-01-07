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
		glClearColor(red, green, blue, alpha);
	}

	void Renderer::ClearBuffers(bool color, bool depth, bool stencil)
	{
		glClear
		(
			(color ? GL_COLOR_BUFFER_BIT : 0) |
			(color ? GL_DEPTH_BUFFER_BIT : 0) |
			(color ? GL_STENCIL_BUFFER_BIT : 0)
		);
	}

	void Renderer::SetCapability(GLCapability capability, bool enable)
	{
		if (enable)
			glEnable(static_cast<GLenum>(capability));
		else
			glDisable(static_cast<GLenum>(capability));
	}

	bool Renderer::IsCapabilityEnabled(GLCapability capability) const
	{
		return glIsEnabled(static_cast<GLenum>(capability));
	}

	void Renderer::SetBlendCapability(BlendingFactors sFactor, BlendingFactors dFactor)
	{
		glBlendFunc(
			static_cast<GLenum>(sFactor),
			static_cast<GLenum>(dFactor)
		);
	}

	void Renderer::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height)
	{
		glViewport(x, y, width, height);
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
		// Top
		DrawLine(Line{
				.p1 = rect.position,
				.p2 = glm::vec2{rect.position.x + rect.width, rect.position.y},
				.color = rect.color
			}
		);

		// Bottom
		DrawLine(
			Line{
				.p1 = glm::vec2{rect.position.x, rect.position.y + rect.height},
				.p2 = glm::vec2{rect.position.x + rect.width, rect.position.y + rect.height},
				.color = rect.color
			}
		);
		// Left
		DrawLine(
			Line{
			.p1 = rect.position,
			.p2 = glm::vec2{rect.position.x, rect.position.y + rect.height},
			.color = rect.color
			}
		);

		// Right
		DrawLine(
			Line{
			.p1 = glm::vec2{rect.position.x + rect.width, rect.position.y},
			.p2 = glm::vec2{rect.position.x + rect.width, rect.position.y + rect.height},
			.color = rect.color
			}
		);
	}

	void Renderer::DrawRect(const glm::vec2& position, float width, float height, const Color& color)
	{
		DrawRect(Rect{ .position = position, .width = width, .height = height, .color = color });
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

	void Renderer::DrawFilledRects(Shader& shader, Camera2D& camera)
	{

	}

	void Renderer::DrawCircles(Shader& shader, Camera2D& camera)
	{

	}

	void Renderer::ClearPrimitives()
	{
		m_Lines.clear();
		m_Rects.clear();
		m_Circles.clear();
	}
}