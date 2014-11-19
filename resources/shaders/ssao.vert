
#version 130

attribute vec3 aCoords;
attribute vec2 aTexUV;

uniform mat4 uMatTransform;

varying vec2 vTexUV;


void main()
{
    gl_Position = vec4 (aCoords.x, aCoords.y, 0, 1);
    vTexUV = vec2 (aTexUV.x, 1 - aTexUV.y);
}
