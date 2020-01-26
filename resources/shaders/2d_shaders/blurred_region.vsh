#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aBackTexCoord;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 proj;

out vec2 BackTexCoord;
out vec2 TexCoord;

void main()
{
    gl_Position = proj * vec4(aPos, 1.0, 1.0);
    BackTexCoord = aBackTexCoord;
    TexCoord = aTexCoord;
}