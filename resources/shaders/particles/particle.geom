
#version 330 core

layout (points)  in;
layout (triangle_strip, max_vertices = 6)  out;

uniform vec3  uBaseUp;
uniform vec3  uBaseRight;

in  vec2  vSize[];
out vec2  gTexUV;


void main()
{
    vec3  up    = uBaseUp * vSize[0].y * 0.5;
    vec3  right = uBaseRight * vSize[0].x * 0.5;
    vec3  pos   = gl_in[0].gl_Position.xyz;
    float w     = gl_in[0].gl_Position.w;

    vec4 leftBottomPos = vec4 (pos - up - right, w);
    vec2 leftBottomUV  = vec2 (0, 1);

    vec4 rightTopPos = vec4 (pos + up + right, w);
    vec2 rightTopUV  = vec2 (1, 0);

    gl_Position = leftBottomPos;
    gTexUV = leftBottomUV;
    EmitVertex();

    gl_Position = vec4 (pos - up + right, w);
    gTexUV = vec2 (1, 1);
    EmitVertex();

    gl_Position = rightTopPos;
    gTexUV = rightTopUV;
    EmitVertex();
    EndPrimitive();

    gl_Position = rightTopPos;
    gTexUV = rightTopUV;
    EmitVertex();

    gl_Position = vec4 (pos + up - right, w);
    gTexUV = vec2 (0, 0);
    EmitVertex();

    gl_Position = leftBottomPos;
    gTexUV = leftBottomUV;
    EmitVertex();
    EndPrimitive();
}
