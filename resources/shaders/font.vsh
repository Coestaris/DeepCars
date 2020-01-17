#version 330 core
uniform vec2 vertices[4];
uniform vec2 texCoords[4];

uniform mat4 proj;

out vec2 TexCoord;

void main()
{
    gl_Position = proj * vec4(vertices[gl_VertexID], 0.0, 1.0);
    TexCoord = texCoords[gl_VertexID];
}