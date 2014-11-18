
#version 130

attribute vec3 aCoords;
attribute vec2 aTexUV;

uniform mat4 uMatTransform;
uniform mat4 uMatWorldTransform;
uniform vec3 uLightPos;
uniform vec3 uViewPos;

varying vec3 vVert2Eye;
varying vec3 vVertWorld;
varying vec2 vTexUV;
varying vec3 vLight2VertPos;
varying vec4 proj_coords;


void main()
{
    gl_Position = uMatTransform * vec4 (aCoords, 1);
    vTexUV = aTexUV * 200;

    vec4 spaceVertex4 = uMatWorldTransform  * vec4 (aCoords, 1);
    vec3 spaceVertex = spaceVertex4.xyz;

    vLight2VertPos = uLightPos ;
    vVert2Eye = uViewPos - spaceVertex;
    proj_coords = gl_Position;
    vVertWorld = spaceVertex;
}
