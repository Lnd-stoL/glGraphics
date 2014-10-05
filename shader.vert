
#version 120

attribute vec3 aCoords;
attribute vec3 aNormal;
attribute vec2 aTexUV;

uniform mat4 uMatTransform;
uniform mat4 uMatWorldTransform;
uniform vec3 uViewPos;

varying vec3 vNormal;
varying vec3 vLight2VertPos;
varying vec2 vTexUV;
varying vec3 vVert2Eye;


void main()
{
    vTexUV = aTexUV;

    gl_Position = uMatTransform * vec4 (aCoords, 1);
    vec4 spaceVertex4 = uMatWorldTransform  * vec4 (aCoords, 1);
    vec3 spaceVertex = spaceVertex4.xyz / spaceVertex4.w;

    vNormal = normalize (mat3 (uMatWorldTransform) * aNormal);
    vLight2VertPos = normalize (vec3 (-10, 50, 50) - spaceVertex);
    vVert2Eye = normalize (uViewPos - spaceVertex);
}
