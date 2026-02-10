#pragma once
#include "Vertex.h"
#include "Font.h"
#include <string>

namespace Scion::Rendering
{
struct Batch
{
	GLuint numIndices{ 0 };
	GLuint offset{ 0 };
	GLuint textureID{ 0 };
};

struct LineBatch
{
	GLuint offset{ 2 };
	GLuint numVertices{ 0 };
	float lineWidth{ 1.f };
};

struct SpriteGlyph
{
	Vertex topLeft;
	Vertex bottomLeft;
	Vertex topRight;
	Vertex bottomRight;
	int layer{ 0 };
	GLuint textureID{ 0 };
};

struct LineGlyph
{
	Vertex p1;
	Vertex p2;
	float lineWidth;
};

struct RectBatch
{
	GLuint numIndices{ 0 };
	GLuint offset{ 0 };
};

struct RectGlyph
{
	Vertex topLeft;
	Vertex bottomLeft;
	Vertex topRight;
	Vertex bottomRight;
};

struct CircleGlyph
{
	CircleVertex topLeft;
	CircleVertex bottomLeft;
	CircleVertex topRight;
	CircleVertex bottomRight;
};

struct TextBatch
{
	GLuint offset{ 0 };
	GLuint numVertices{ 0 };
	GLuint fontAtlasID{ 0 };
};

struct TextGlyph
{
	std::string textStr{};
	glm::vec2 position{ 0.f };
	Color color{ 255, 255, 255, 255 };
	glm::mat4 model{ 1.f };
	std::shared_ptr<Font> font{ nullptr };
	float wrap{ 0.f };
	int padding{ 0 };
};

struct PickingGlyph
{
	PickingVertex topLeft{};
	PickingVertex bottomLeft{};
	PickingVertex topRight{};
	PickingVertex bottomRight{};
	int layer{ 0 };
	GLuint textureID{ 0 };
};

} // namespace Scion::Rendering
