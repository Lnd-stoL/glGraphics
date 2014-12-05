
#version 330 core

uniform mat4  uMatTransform;

in vec3 aPosition;
in vec2 aSize;

out vec2 vSize;


void main()
{
    gl_Position = uMatTransform * vec4 (aPosition, 1);
    vSize = aSize;
}
