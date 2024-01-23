#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uvs;

uniform mat4 uProjection;

out vec4 vertexColor;
out vec2 vertexUVs;

void main()
{
	gl_Position = uProjection * vec4(position.x, position.y, 0.0, 1.0);
	vertexColor = color;
	vertexUVs = uvs;
}