#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = vec4(1,0,1,0); // texture(skybox, TexCoords);
}