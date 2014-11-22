
#version 330 core

in vec3 aCoords;
in vec3 aNormal;
in vec2 aTexUV;

uniform mat4 uMatTransform;
uniform mat4 uMatWorldTransform;
uniform mat4 uMatWorldViewTransform;
uniform vec3 uViewPos;
uniform mat4 uShadowmapTransform;
uniform vec3 uLightPos;

out vec3 vNormal;
out vec3 vLight2VertPos;
out vec2 vTexUV;
out vec3 vVert2Eye;
out vec4 vShadowmapVert;
out vec3 vViewSpaceNormal;
out vec3 vViewSpaceCoords;


void main()
{
    vTexUV = aTexUV;

    gl_Position = uMatTransform * vec4 (aCoords, 1);

    vec4 spaceVertex4 = uMatWorldTransform  * vec4 (aCoords, 1);
    vec3 spaceVertex = spaceVertex4.xyz / spaceVertex4.w;

    gl_ClipDistance[0] = spaceVertex.y - 1.01;
    //gl_ClipVertex = spaceVertex4;

    vNormal = mat3 (uMatWorldTransform) * aNormal;
    vViewSpaceNormal = mat3 (uMatWorldViewTransform) * aNormal;
    vLight2VertPos = uLightPos - spaceVertex;
    vVert2Eye = uViewPos - spaceVertex;

    vShadowmapVert = uShadowmapTransform * vec4 (aCoords, 1);
    vViewSpaceCoords = (uMatWorldViewTransform * vec4 (aCoords, 1)).xyz;
}
