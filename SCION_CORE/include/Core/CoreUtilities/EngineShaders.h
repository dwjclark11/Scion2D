#pragma once

namespace SCION_CORE::Shaders
{
static const char* basicShaderVert = R"(
#version 450 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec4 aColor;

out vec2 fragUVs;
out vec4 fragColor;
uniform mat4 uProjection;

void main()
{
	gl_Position = uProjection * vec4(aPosition, 0.0, 1.0);
	fragUVs = aTexCoords;
	fragColor = aColor;
}
)";

static const char* basicShaderFrag = R"(
#version 450 core

in vec2 fragUVs;
in vec4 fragColor;
out vec4 color;
uniform sampler2D uTexture;

void main()
{
	vec4 textureColor = texture(uTexture, fragUVs);
	color = textureColor * fragColor; 
}
)";

static const char* circleShaderVert = R"(
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
)";

static const char* circleShaderFrag = R"(
#version 450 

in vec2 fragUVs;
in vec4 fragColor;
in float fragLineThickness;

out vec4 color;

void main()
{
	// length calculates the length of the given vector
	float distance = 1 - length(fragUVs);

	// fwidth — return the sum of the absolute value of derivatives in x and y
	float wd = fwidth(distance);

	// smoothstep will perform an interpolation between the two values
	float circle = smoothstep(0, wd, distance); 

	circle *= smoothstep(fragLineThickness + wd, fragLineThickness, distance);
    
	if (circle == 0.0)
		discard;

	color = fragColor;
	color.a *= circle;
}
)";

static const char* fontShaderVert = R"(
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
)";

static const char* fontShaderFrag = R"(
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
)";

static const char* colorShaderVert = R"(
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
)";

static const char* colorShaderFrag = R"(
#version 450

in vec4 fragmentColor;

out vec4 color;

void main()
{
	color = fragmentColor;
}
)";

	static const char* pickingShaderVert = R"(
#version 450 

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in int inEntityID;

out vec2 fragmentUV;
flat out int outEntityID;

uniform mat4 uProjection;

void main()
{
	gl_Position = uProjection *  vec4(vertexPosition, 0.0, 1.0);
	
	fragmentUV = vertexUV;
	outEntityID = inEntityID;
}
)";

static const char* pickingShaderFrag = R"(
#version 450

in vec2 fragmentUV;
flat in int outEntityID;

out int color;

uniform sampler2D uSpriteTexture;

void main()
{
	
	vec4 textureColor = texture(uSpriteTexture, fragmentUV);

	if (textureColor.a < 0.5)
	{
		discard;
	}

	color = outEntityID;
}
)";

}

