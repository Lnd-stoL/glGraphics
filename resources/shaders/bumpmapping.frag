
#version 330 core

#with  common/shadowmapping.glsl

//----------------------------------------------------------------------------------------------------------------------

uniform sampler2D  uTxtDiffuse;
uniform sampler2D  uTxtNormalmap;

uniform sampler2DShadow  uTxtShadowMap;

uniform vec3  uLightColor;
uniform vec3  uLightPos;

//----------------------------------------------------------------------------------------------------------------------

in vec3  gNormal;
in vec3  gLight2Vert;
in vec2  gTexUV;
in vec3  gVert2Eye;
in vec4  gShadowmapVert;
in vec3  gViewSpaceNormal;

in vec3  gTangent;
in vec3  gBitangent;
in vec3  gFaceNormal;

//----------------------------------------------------------------------------------------------------------------------

out vec3  oColor;
out vec3  oNormal;

//----------------------------------------------------------------------------------------------------------------------

void main()
{
    vec3 smoothNormal = normalize (gNormal);
    vec3 light2Vert   = normalize (gLight2Vert);
    vec3 vert2Eye     = normalize (gVert2Eye);

    mat3 matTBN = mat3 (normalize (gTangent), normalize (gBitangent), smoothNormal);
    vec3 diffColor  = texture (uTxtDiffuse, gTexUV).xyz;

    float normalXlight2Vert = dot (smoothNormal, light2Vert);
    float shadow = shadowmapShading (gShadowmapVert, uTxtShadowMap, normalXlight2Vert);

    vec3 mappedNormal = texture (uTxtNormalmap, gTexUV).rgb * 2 - 1;
    vec3 normal = normalize (matTBN * mappedNormal);

    float diffLight = max (0, dot (normal, light2Vert));
    //vec3 spec = vec3 (uLightColor) * pow (max (dot (light2Vert, reflect (-vert2Eye, nnormal)), 0), 50);
    oColor = ((diffLight*0.8 + 0.2)) * uLightColor * diffColor * shadow;

    oNormal = vec3 (gViewSpaceNormal / 2 + 0.5);
}
