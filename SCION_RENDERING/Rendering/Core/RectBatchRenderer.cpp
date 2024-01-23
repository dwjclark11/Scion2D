#include "RectBatchRenderer.h"
#include "../Essentials/Primitives.h"

namespace SCION_RENDERING {

	void RectBatchRenderer::GenerateBatches()
	{
		std::vector<Vertex> vertices;
		vertices.resize(m_Glyphs.size() * 4);

		int currentVertex{ 0 };

		m_Batches.push_back(std::make_shared<RectBatch>(RectBatch{ .numIndices = 6, .offset = 0 }));

		for (const auto& shape : m_Glyphs)
		{
			vertices[currentVertex++] = shape->topLeft;
			vertices[currentVertex++] = shape->topRight;
			vertices[currentVertex++] = shape->bottomRight;
			vertices[currentVertex++] = shape->bottomLeft;

			m_Batches.back()->numIndices += 6;
		}

		glBindBuffer(GL_ARRAY_BUFFER, GetVBO());
		// Orphan the buffer
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
		// Upload the data
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void RectBatchRenderer::Initialize()
	{
		SetVertexAttribute(0, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
		SetVertexAttribute(1, 4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, color), GL_TRUE);
	}

	RectBatchRenderer::RectBatchRenderer()
		: Batcher(true)
	{
		Initialize();
	}

	void RectBatchRenderer::End()
	{
		// If the glyphs are empty, there is nothing to render
		if (m_Glyphs.empty())
			return;

		GenerateBatches();
	}

	void RectBatchRenderer::Render()
	{
		EnableVAO();

		for (const auto& batch : m_Batches)
		{
			glDrawElements(GL_TRIANGLES, batch->numIndices, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * batch->offset));
		}

		DisableVAO();
	}

	void RectBatchRenderer::AddRect(const glm::vec4& destRect, int layer, const Color& color)
	{
		std::shared_ptr<RectGlyph> newGlyph = std::make_shared<RectGlyph>();

		newGlyph->topLeft.color = color;
		newGlyph->topLeft.position = glm::vec2{ destRect.x, destRect.y + destRect.w };

		newGlyph->bottomLeft.color = color;
		newGlyph->bottomLeft.position = glm::vec2{ destRect.x, destRect.y };

		newGlyph->bottomRight.color = color;
		newGlyph->bottomRight.position = glm::vec2{ destRect.x + destRect.z, destRect.y };

		newGlyph->topRight.color = color;
		newGlyph->topRight.position = glm::vec2{ destRect.x + destRect.z, destRect.y + destRect.w };

		m_Glyphs.push_back(std::move(newGlyph));
	}

	void RectBatchRenderer::AddRect(const Rect& rect)
	{
		std::shared_ptr<RectGlyph> newGlyph = std::make_shared<RectGlyph>();

		newGlyph->topLeft.color = rect.color;
		newGlyph->topLeft.position = glm::vec2{ rect.position.x, rect.position.y + rect.height };

		newGlyph->bottomLeft.color = rect.color;
		newGlyph->bottomLeft.position = glm::vec2{ rect.position.x, rect.position.y };

		newGlyph->bottomRight.color = rect.color;
		newGlyph->bottomRight.position = glm::vec2{ rect.position.x + rect.width, rect.position.y };

		newGlyph->topRight.color = rect.color;
		newGlyph->topRight.position = glm::vec2{ rect.position.x + rect.width, rect.position.y + rect.height };

		m_Glyphs.push_back(std::move(newGlyph));
	}
}