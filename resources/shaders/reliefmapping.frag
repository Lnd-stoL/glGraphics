
#version 330 core

#with  common/shadowmapping.glsl

//----------------------------------------------------------------------------------------------------------------------

uniform sampler2D  uTxtDiffuse;
uniform sampler2D  uTxtNormalmap;
uniform sampler2D  uTxtHeightmap;

uniform sampler2DShadow  uTxtShadowMap;
uniform sampler2D  uTxtShadowMapFlat;

uniform vec3 uLightColor;
uniform vec3 uLightPos;

//----------------------------------------------------------------------------------------------------------------------

in vec3  gNormal;
in vec3  gLight2Vert;
in vec2  gTexUV;
in vec3  gVert2Eye;
in vec4  gShadowmapVert;
in vec3  gViewSpaceNormal;
in vec3  gWSC;

in vec3  gTangent;
in vec3  gBitangent;
in vec3  gFaceNormal;
in vec3  gVert2EyeTBN;

//----------------------------------------------------------------------------------------------------------------------

out vec3  oColor;
out vec3  oNormal;

//----------------------------------------------------------------------------------------------------------------------

const float _shiftScale       = 0.07;
const int   _rayTracingSteps  = 16;

//----------------------------------------------------------------------------------------------------------------------

vec3 perturbNormal (mat3 matTBN, vec2 texcoord)
{
    vec3 map = texture (uTxtNormalmap, texcoord).rgb * 2 - 1;
    return normalize (matTBN * map);
}


vec2 calcUVShifting()
{
    vec3 vert2EyeCtg = normalize (gVert2EyeTBN);

    vec2 shiftedUV = gTexUV * 1.7;
    vec2 deltaUV = vert2EyeCtg.xy * _shiftScale / (_rayTracingSteps * vert2EyeCtg.z);
    float stepLength = 1.0 / float (_rayTracingSteps);
    float projHeight = 1.0;

    float nextHeight = texture (uTxtHeightmap, shiftedUV).x;
    while (projHeight >= nextHeight)
    {
        projHeight -= stepLength;
        shiftedUV  += deltaUV;

        nextHeight = texture (uTxtHeightmap, shiftedUV).x + 0.01;
    }

    vec2  prevDelta = shiftedUV - deltaUV;
    float prevHeight = texture (uTxtHeightmap, prevDelta).x - (projHeight + stepLength);
    float curHeight = nextHeight - projHeight;
    float linearCoeff = curHeight / (curHeight - prevHeight);
    shiftedUV = (1.0 - linearCoeff) * shiftedUV + linearCoeff * prevDelta;

    return shiftedUV;
}


void main()
{
    vec3 light2Vert   = normalize (uLightPos - gWSC);
    vec3 vert2Eye     = normalize (gVert2Eye);

    vec3 normal = normalize (gFaceNormal);
    mat3 matTBN = mat3 (normalize (gTangent), normalize (gBitangent), normal);
    vec3 vert2EyeCtg = normalize (gVert2EyeTBN);

    vec2 shiftedUV = calcUVShifting();

    vec3 diffColor  = texture (uTxtDiffuse, shiftedUV).xyz;
    normal = perturbNormal (matTBN, shiftedUV);

    float diffLight = max (0, dot (normal, light2Vert));
    vec3 spec = vec3 (uLightColor) * pow (max (dot (light2Vert, reflect (-vert2Eye, normal)), 0), 50);
    oColor = ((diffLight*0.7 + 0.3)) * uLightColor * diffColor + spec;

    float normalXlight2Vert = 0.5;
    float shadow = shadowmapShading (gShadowmapVert, uTxtShadowMap, normalXlight2Vert);
    oColor *= shadow;

    oNormal = vec3 (gViewSpaceNormal / 2 + 0.5);
}
