#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;

void main()
{
/*    const float gamma = 1;
    const float exposure = 1;

    vec3 hdrColor = texture(tex, TexCoord).rgb;

    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));*/

    //FragColor = vec4(mapped, 1.0);
    FragColor = vec4(texture(tex, TexCoord).rgb, 1.0);
}