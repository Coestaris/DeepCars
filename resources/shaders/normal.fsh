#version 330 core
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 color;

void main()
{
    gAlbedoSpec = vec4(color, 0);
}