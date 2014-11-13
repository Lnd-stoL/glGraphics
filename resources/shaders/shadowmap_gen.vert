
#version 130

attribute vec3 aCoords;

uniform mat4 uMatTransform;


void main()
{
    gl_Position = uMatTransform * vec4 (aCoords, 1);
}
