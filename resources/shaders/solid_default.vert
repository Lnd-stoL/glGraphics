
#version 330 core

//----------------------------------------------------------------------------------------------------------------------

in vec3  iCoords;
in vec3  iNormal;
in vec2  iTexUV;

//----------------------------------------------------------------------------------------------------------------------

uniform mat4  uMatTransform;
uniform mat4  uMatWorldTransform;
uniform mat4  uMatWorldViewTransform;

uniform vec3  uViewPos;
uniform vec3  uLightPos;

uniform mat4  uMatShadowmapTransform;
uniform float uWaterClipHeight = 1.1;

//----------------------------------------------------------------------------------------------------------------------

out vec3  vNormal;
out vec3  vLight2Vert;
out vec2  vTexUV;
out vec3  vVert2Eye;

out vec4  vShadowmapVert;
out vec3  vViewSpaceNormal;
out vec3  vWorldSpaceCoords;

//----------------------------------------------------------------------------------------------------------------------

void main()
{
    vTexUV = iTexUV;

    vec4 coords4 = vec4 (iCoords, 1);
    gl_Position = uMatTransform * coords4;

    vec3 worldCoords = (uMatWorldTransform  * coords4).xyz;
    vWorldSpaceCoords = worldCoords;
    vNormal = mat3 (uMatWorldTransform) * iNormal;

    vLight2Vert = uLightPos - worldCoords;
    vVert2Eye   = uViewPos  - worldCoords;

    vViewSpaceNormal = mat3 (uMatWorldViewTransform) * iNormal;
    vShadowmapVert = uMatShadowmapTransform * coords4;

    gl_ClipDistance[0] = worldCoords.y - uWaterClipHeight - 0.01;
}
