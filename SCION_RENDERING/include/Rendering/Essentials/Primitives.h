#pragma once
#include "Vertex.h"
#include "Font.h"
#include <string>
#include <memory>

namespace SCION_RENDERING
{

struct Line
{
	glm::vec2 p1{ 0.f };
	glm::vec2 p2{ 0.f };
	float lineWidth{ 1.f };
	Color color{};
};

struct Rect
{
	glm::vec2 position{ 0.f };
	float width{ 0.f };
	float height{ 0.f };
	Color color{};
};

struct Circle
{
	glm::vec2 position{ 0.f };
	float lineThickness{ 1.f };
	float radius{ 0.f };
	Color color{};
};

struct Text
{
	glm::vec2 position{ 0.f };
	std::string textStr{ "" };
	float wrap{ -1.f };
	std::shared_ptr<Font> pFont{ nullptr };
	Color color{ 255, 255, 255, 255 };
};

} // namespace SCION_RENDERING
