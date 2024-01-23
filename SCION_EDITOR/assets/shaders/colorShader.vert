#version 450

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec4 vertexColor;

out vec4 fragmentColor;
uniform mat4 uProjection;

void main()
{
	gl_Position.xy = (uProjection * vec4(vertexPosition, 0.0, 1.0)).xy;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;

	fragmentColor = vertexColor;
}