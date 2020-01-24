#version 330 core
out float FragColor;

in vec2 TexCoord;

uniform sampler2D ssaoInput;

const int radius = 2;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -radius; x < radius; ++x)
    {
        for (int y = -radius; y < radius; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, TexCoord + offset).r;
        }
    }
    FragColor = result / (4 * radius * radius);
}