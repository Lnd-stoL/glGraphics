
#version 330 core

#with  common/shadowmapping.glsl

//----------------------------------------------------------------------------------------------------------------------

uniform sampler2D        uTxtDiffuse;
uniform sampler2DShadow  uTxtShadowMap;

uniform vec3  uLightColor;
uniform vec3  uLightPos;

//----------------------------------------------------------------------------------------------------------------------

in vec3  vNormal;
in vec3  vLight2Vert;
in vec2  vTexUV;
in vec3  vVert2Eye;

in vec4  vShadowmapVert;
in vec3  vViewSpaceNormal;

//----------------------------------------------------------------------------------------------------------------------

out vec3  oColor;
out vec3  oNormal;

//----------------------------------------------------------------------------------------------------------------------

void main()
{
    vec3 normal     = normalize (vNormal);
    vec3 light2Vert = normalize (vLight2Vert);
    vec3 vert2Eye   = normalize (vVert2Eye);

    float normalXlight2Vert = dot (normal, light2Vert);
    vec3 diffColor = texture2D (uTxtDiffuse, vTexUV).xyz;
    float diffLightness = max (0, normalXlight2Vert);
    float ambientLightness = 0.2;

    float shadow = shadowmapShading (vShadowmapVert, uTxtShadowMap, normalXlight2Vert);

    vec3 totalColor = uLightColor * diffColor;
    float totalLightness = diffLightness * shadow * (1 - ambientLightness) + ambientLightness;
    oColor = totalLightness * totalColor;

    oNormal = vViewSpaceNormal / 2 + 0.5;
}
