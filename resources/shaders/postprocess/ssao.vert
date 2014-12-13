
#version 330 core

in vec3 aCoords;
in vec2 aTexUV;

out vec2 vTexUV;


void main()
{
    gl_Position = vec4 (aCoords.x, aCoords.y, 0, 1);
    vTexUV = vec2 (aTexUV.x, 1 - aTexUV.y);
}
