#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;

void main()
{
    //float depthValue = texture(tex, TexCoord).r;
    //FragColor = vec4(vec3(depthValue), 1.0);

    FragColor = texture(tex, TexCoord);
}