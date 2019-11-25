#version 330 core

out vec3 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 objectColor;

void main()
{
    FragColor = objectColor;
}