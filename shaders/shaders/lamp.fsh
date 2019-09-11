#version 330 core

out vec3 FragColor;

uniform vec3 lightColor;

void main()
{
    FragColor = lightColor;
}