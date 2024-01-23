#version 450 core

in vec4 vertexColor;
in vec2 vertexUVs;

uniform sampler2D atlas;

out vec4 finalColor;

void main()
{
	vec4 pixelColor = vec4(1, 1, 1, texture(atlas, vertexUVs).r);
	finalColor = pixelColor * vertexColor;
}
