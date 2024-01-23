#version 450

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aUVs;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aLineThickness;

out vec2 fragUVs;
out vec4 fragColor;
out float fragLineThickness;

uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * vec4(aPosition, 0.0, 1.0);
    fragUVs = aUVs;
    fragColor = aColor;
    fragLineThickness = aLineThickness;
}