#version 330 core

out vec3 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 objectColor;
uniform vec3 viewerPosition;
uniform float ambient;

void main()
{
    vec3 dir = normalize(viewerPosition - FragPos);
    float k = (abs(dot(dir, Normal)) + ambient);
    if(k > 1) k = 1;

    FragColor = objectColor * k;
}