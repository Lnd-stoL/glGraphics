
#version 130

attribute vec3 aCoords;
attribute vec3 aNormal;
attribute vec2 aTexUV;

uniform mat4 uMatTransform;


void main()
{
    gl_Position = uMatTransform * vec4 (aCoords, 1);
}
