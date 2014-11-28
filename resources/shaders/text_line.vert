
#version 330 core

in vec2  aCoords;
in vec2  aTexUV;

out vec2 vTexUV;

uniform vec2  uOffset;
uniform vec2  uScale;


void main()
{
    gl_Position = vec4 (aCoords * uScale + uOffset, 0, 1);
    vTexUV = aTexUV;
}
