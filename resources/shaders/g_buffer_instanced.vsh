#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec3 ViewFragPos;
out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 viewModel = view * aInstanceMatrix;
    ViewFragPos = vec3(viewModel * vec4(aPos, 1.0));
    FragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));

    TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(viewModel)));
    Normal = normalMatrix * aNormal;

    gl_Position = projection * viewModel * vec4(aPos, 1.0);
}