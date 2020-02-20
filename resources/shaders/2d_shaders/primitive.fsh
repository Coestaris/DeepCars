#version 330 core
out vec4 color;

uniform vec3 primitive_color;

void main()
{
    color = vec4(primitive_color, 1.0f);
}