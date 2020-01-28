#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 BackTexCoord;

uniform sampler2D backTex;
uniform sampler2D tex;

uniform int kSize;
uniform float kernel[32];
uniform float z;

uniform vec3 grayscale_color;

uniform float transparency;

void main(void)
{
    vec2 texelSize = 1.0 / vec2(textureSize(backTex, 0));
    vec3 result = vec3(0);

    for (int i= -kSize; i <= kSize; ++i)
        for (int j= -kSize; j <= kSize; ++j)
        {
            vec2 offset = vec2(float(i), float(j)) * texelSize;
            result +=
                kernel[kSize + j] *
                kernel[kSize + i] * texture(backTex, BackTexCoord + offset).rgb;
        }
    result /= (z * z);

    result = vec3(result.r * 0.3 + result.g * 0.59 + result.b * 0.11) * grayscale_color;

    vec4 tex_color = texture(tex, TexCoord);
    FragColor = mix(texture(backTex, BackTexCoord), vec4(mix(tex_color.rgb, result, 1-tex_color.a), 1), transparency);
}