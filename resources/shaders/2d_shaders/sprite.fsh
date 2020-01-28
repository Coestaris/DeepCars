#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform float transparency;

void main()
{
    vec4 c = texture(image, TexCoords);
    c.a *= 1 - transparency;

    color = c;
}