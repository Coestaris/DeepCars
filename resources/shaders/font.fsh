#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;
uniform vec4 color;
uniform vec4 params;
uniform vec3 borderColor;

void main(void)
{
    float tx=texture2D(tex, TexCoord).r;
    float b=min((tx-params.x)*params.y, 1.0);
    float a=clamp((tx-params.z)*params.w, 0.0, 1.0);

    if(a == 1)
        discard;

    //gl_FragColor=vec4(color.rgb,1-a);
    gl_FragColor=vec4(color.rgb+(borderColor-color.rgb)*a, b);
}