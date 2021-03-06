#version 330 core

//#define RENDER_UV

layout (location = 0) out vec3 gViewPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gPosition;

in vec3 ViewFragPos;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main()
{
    gViewPosition = ViewFragPos;
    gPosition = FragPos;

    gNormal = normalize(Normal);

#ifdef RENDER_UV
    gAlbedoSpec.rgb = vec3(TexCoords, 0);
#else
    gAlbedoSpec.rgb = texture(texture_diffuse, TexCoords).rgb;
#endif

    gAlbedoSpec.a = texture(texture_specular, TexCoords).r;
}