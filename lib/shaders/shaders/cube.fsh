#version 330 core
out vec3 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;

void main()
{
    //FragColor =  texture(material.diffuse, TexCoords).rgb;
    float d = dot(Normal);
    FragColor =  vec3(0.4f);
}