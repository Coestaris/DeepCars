#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;
uniform vec4 color;
uniform vec4 params;
uniform vec3 borderColor;

void main(void)
{
    float tx = texture2D(tex, TexCoord).r;
    float b = min((tx - params.z) * params.w, 1.0);
    float a = clamp((tx - params.x) * params.y, 0.0, 1.0);

    gl_FragColor = vec4(borderColor + (color.rgb - borderColor) * a, b * color.a);
}