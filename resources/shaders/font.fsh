#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;
uniform vec4 color;
uniform vec2 params;
//uniform vec3 borderColor;

void main(void)
{
    float tx=texture2D(tex, TexCoord).r;
    float a=min((tx-params.x)*params.y, 1.0);
    if(a == 1)
        discard;
    gl_FragColor=vec4(color.rgb,1-a);
}