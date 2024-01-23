#version 450 

in vec2 fragUVs;
in vec4 fragColor;
in float fragLineThickness;

out vec4 color;

void main()
{
    float distance = 1 - length(fragUVs); // length calculates the length of the given vector
    float wd = fwidth(distance); // fwidth — return the sum of the absolute value of derivatives in x and y
    float circle = smoothstep(0, wd, distance); // smoothstep will perform an interpolation between the two values

    circle *= smoothstep(fragLineThickness + wd, fragLineThickness, distance);
    
    if (circle == 0.0)
        discard;

    color = fragColor;
    color.a *= circle;
}

