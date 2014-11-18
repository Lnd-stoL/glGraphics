
#version 130

attribute vec3 aCoords;
attribute vec3 aNormal;
attribute vec2 aTexUV;

uniform mat4 uMatTransform;
uniform mat4 uMatWorldTransform;
uniform vec3 uViewPos;
uniform mat4 uShadowmapTransform;
uniform vec3 uLightPos;

varying vec3 vNormal;
varying vec3 vLight2VertPos;
varying vec2 vTexUV;
varying vec3 vVert2Eye;
varying vec4 vShadowmapVert;


void main()
{
    vTexUV = aTexUV;

    gl_Position = uMatTransform * vec4 (aCoords, 1);

    vec4 spaceVertex4 = uMatWorldTransform  * vec4 (aCoords, 1);
    vec3 spaceVertex = spaceVertex4.xyz / spaceVertex4.w;

    //gl_ClipDistance[0] = spaceVertex.y - 1.01;
    gl_ClipVertex = spaceVertex4;

    vNormal = mat3 (uMatWorldTransform) * aNormal;
    vLight2VertPos = uLightPos - spaceVertex;
    vVert2Eye = uViewPos - spaceVertex;

    vShadowmapVert = uShadowmapTransform * vec4 (aCoords, 1);
}
