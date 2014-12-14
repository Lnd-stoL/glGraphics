
#version 330 core

in vec3 iCoords;
in vec2 iTexUV;

uniform mat4  uMatTransform;
uniform mat4  uMatWorldTransform;
uniform vec3  uLightPos;
uniform vec3  uViewPos;

out vec3  vVert2Eye;
out vec3  vVertWorld;
out vec2  vTexUV;
out vec3  vLight2VertPos;
out vec4  proj_coords;


void main()
{
    gl_Position = uMatTransform * vec4 (iCoords, 1);
    vTexUV = iTexUV * 170;

    vec4 spaceVertex4 = uMatWorldTransform  * vec4 (iCoords, 1);
    vec3 spaceVertex = spaceVertex4.xyz;

    vLight2VertPos = uLightPos ;
    vVert2Eye = uViewPos - spaceVertex;
    proj_coords = gl_Position;
    vVertWorld = spaceVertex;
}
