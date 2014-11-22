
#version 330 core

in vec3 aCoords;
in vec2 aTexUV;
in vec3 aNormal;

uniform mat4 uMatTransform;

out vec3 vTexCube;


void main()
{
    gl_Position = uMatTransform * vec4 (vec3 (aCoords.x, aCoords.y, aCoords.z) * 500, 1);
    gl_ClipDistance[0] = 1;
    vTexCube = vec3 (aCoords.x, aCoords.yz);
}
