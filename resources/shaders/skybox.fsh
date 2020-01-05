#version 330 core
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    gAlbedoSpec = texture(skybox, TexCoords);
}