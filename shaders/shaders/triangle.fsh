#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform float mixLevel;

void main()
{
    FragColor = vec4(mix(texture(ourTexture, TexCoord).xyz, ourColor, mixLevel), 0);
}