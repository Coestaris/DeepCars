#version 330 core

out vec3 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D tex;
uniform vec3 viewerPosition;

void main()
{
    vec3 dir = normalize(viewerPosition - FragPos);
    FragColor = texture(tex, TexCoords).xyz * dot(dir, Normal);
    //FragColor = vec3(TexCoords.xy, 0);
}