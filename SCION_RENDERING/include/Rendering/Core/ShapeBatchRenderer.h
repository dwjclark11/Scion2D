#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "Rendering/Essentials/Vertex.h"

namespace Scion::Rendering
{
struct ShapeBatch
{
	GLuint numIndices{ 0 };
	GLuint offset{ 0 };
	int layer{ 0 };
};

struct ShapeGlyph
{
	int layer{ 0 };
	bool bIsLine{ false };

	uint32_t vertexOffset{ 0 };
	uint32_t vertexCount{ 0 };

	uint32_t indexOffset{ 0 };
	uint32_t indexCount{ 0 };
};

class ShapeRenderer
{
  public:
	ShapeRenderer();
	~ShapeRenderer();

	void Begin();
	void End();
	void Render();

	bool Empty() const { return m_ShapeGlyphs.empty(); }

	static int CircleSegments( float radius );

	// Solid
	// Centered on the model origin
	void AddRectangle( const glm::vec2& size, const glm::mat4& model, const Color& color, int layer = 0 );

	// position is the top left corner in world space, model is applied on top of that
	void AddRect( const glm::vec2& position, float width, float height, const glm::mat4& model, const Color& color,
				  int layer = 0 );
	void AddRect( const glm::vec2& position, float width, float height, const Color& color, int layer = 0 );

	// Four points in winding order, also used for iso (diamond) rects
	void AddQuad( const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3,
				  const glm::mat4& model, const Color& color, int layer = 0 );

	void AddLine( const glm::vec2& p1, const glm::vec2& p2, float width, const Color& color, int layer = 0 );

	// Outline is centered on the rect edges
	void AddRectOutline( const glm::vec2& position, float width, float height, float thickness, const Color& color,
						 int layer = 0 );

	void AddTriangle( const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::mat4& model,
					  const Color& color, int layer = 0 );

	void AddCircle( float radius, int segments, const glm::mat4& model, const Color& color, int layer = 0 );

	void AddRing( float outerRadius, float innerRadius, int segments, const glm::mat4& model, const Color& color,
				  int layer = 0 );

	void AddPolygon( const std::vector<glm::vec2>& points, const glm::mat4& model, const Color& color, int layer = 0 );

  private:
	void Initialize();
	void CreateBatches();
	glm::vec2 TransformPoint( const glm::vec2& p, const glm::mat4& model );
	void PushQuad( const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3,
				   const Color& color, int layer, bool bIsLine = false );

  private:
	GLuint m_VAO{};
	GLuint m_VBO{};
	GLuint m_EBO{};

	std::vector<ShapeVertex> m_Vertices;
	std::vector<GLuint> m_Indices;

	std::vector<ShapeGlyph> m_ShapeGlyphs;
	std::vector<ShapeBatch> m_Batches;
};

} // namespace Scion::Rendering
