#version 330

layout (lines) in;                              // now we can access 2 vertices
layout (triangle_strip, max_vertices = 4) out;  // always (for now) producing 2 triangles (so 4 vertices)

uniform vec2    u_viewportInvSize;
uniform mat4    view;
uniform float   u_thickness = 1;

void main()
{
    float r = u_thickness;

    vec4 p1 = gl_in[0].gl_Position;
    vec4 p2 = gl_in[1].gl_Position;

    vec2 dir = normalize(p2.xy - p1.xy);
    vec2 normal = vec2(dir.y, -dir.x);

    vec4 offset1, offset2;
    offset1 = vec4(normal * (r), 0, 0);
    offset2 = vec4(normal * (-r), 0, 0);

    vec4 coords[4];
    coords[0] = p1 + offset1;
    coords[1] = p1 - offset1;
    coords[2] = p2 + offset2;
    coords[3] = p2 - offset2;

    for (int i = 0; i < 4; ++i) {
        gl_Position = view * coords[i];
        EmitVertex();
    }
    EndPrimitive();
}