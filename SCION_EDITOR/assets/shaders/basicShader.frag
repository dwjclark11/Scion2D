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