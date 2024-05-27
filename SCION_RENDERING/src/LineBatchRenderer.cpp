#include "Rendering/Core/LineBatchRenderer.h"

namespace SCION_RENDERING {

	void LineBatchRenderer::GenerateBatches()
	{
		std::vector<Vertex> vertices;
		vertices.resize(m_Glyphs.size() * 2);

		int currentVertex{ 0 };

		m_Batches.push_back(std::make_shared<LineBatch>(LineBatch{ .offset = 0, .numVertices = 2 }));

		for (const auto& line : m_Glyphs)
		{			
			vertices[currentVertex++] = line->p1;
			vertices[currentVertex++] = line->p2;
			m_Batches.back()->lineWidth = line->lineWidth;

			if (m_Glyphs.size() == 1)
				break;

			m_Batches.back()->numVertices += 2;
		}

		glBindBuffer(GL_ARRAY_BUFFER, GetVBO());

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void LineBatchRenderer::Initialize()
	{
		SetVertexAttribute(0, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
		SetVertexAttribute(1, 4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, color), GL_TRUE);
	}

	LineBatchRenderer::LineBatchRenderer()
		: Batcher(false)
	{
		Initialize();
	}

	void LineBatchRenderer::End()
	{
		if (m_Glyphs.empty())
			return;

		GenerateBatches();
	}

	void LineBatchRenderer::Render()
	{
		glEnable(GL_LINE_SMOOTH);
		EnableVAO();
		for (const auto& batch : m_Batches)
		{
			glDrawArrays(GL_LINES, 0, batch->numVertices);
		}
		DisableVAO();
		glDisable(GL_LINE_SMOOTH);
	}

	void LineBatchRenderer::AddLine(const Line& line)
	{
		std::shared_ptr<LineGlyph> newGlyph = std::make_shared<LineGlyph>(
			LineGlyph{
				.p1 = Vertex{
					.position = line.p1,
					.color = line.color
				},
				.p2 = Vertex {
					.position = line.p2,
					.color = line.color
				},
				.lineWidth = line.lineWidth,				
			}
		);
			
		m_Glyphs.push_back(std::move(newGlyph));
	}
}