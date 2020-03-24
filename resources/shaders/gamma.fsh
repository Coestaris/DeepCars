#version 330 core
#extension GL_ARB_shading_language_420pack : require

#define radius 2

out vec3 FragColor;

in vec2 TexCoord;

uniform bool postprocess;
uniform float kernel[25];
uniform float z;
uniform sampler2D depth_tex;
uniform sampler2D tex;

void main()
{
    if(!postprocess)
    {
        FragColor = texture(tex, TexCoord).rgb;
        return;
    }

    vec3 tex_color = texture(tex, TexCoord).rgb;
    float depth = clamp(-texture(depth_tex, TexCoord).z / 250 - 0.3, 0, 1);

    vec2 texelSize = 1.0 / vec2(textureSize(tex, 0));
    vec3 result = vec3(0);

    for (int i= -radius; i <= radius; ++i)
    for (int j= -radius; j <= radius; ++j)
    {
        vec2 offset = vec2(float(i), float(j)) * texelSize;
        result +=
            kernel[radius + j] *
            kernel[radius + i] * (texture(tex, TexCoord + offset).rgb + vec3(depth / 2));
    }

    result /= (z * z);

    vec3 mixed = mix(tex_color, result, depth).rgb;
    FragColor = mixed;
}