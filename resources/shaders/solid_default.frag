
#version 330 core

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

const vec2 cPoissonDistribution16[] = vec2[] (
    vec2( -0.94201624,  -0.39906216 ),
    vec2(  0.94558609,  -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2(  0.34495938,   0.29387760 ),
    vec2( -0.91588581,   0.45771432 ),
    vec2( -0.81544232,  -0.87912464 ),
    vec2( -0.38277543,   0.27676845 ),
    vec2(  0.97484398,   0.75648379 ),
    vec2(  0.44323325,  -0.97511554 ),
    vec2(  0.53742981,  -0.47373420 ),
    vec2( -0.26496911,  -0.41893023 ),
    vec2(  0.79197514,   0.19090188 ),
    vec2( -0.24188840,   0.99706507 ),
    vec2( -0.81409955,   0.91437590 ),
    vec2(  0.19984126,   0.78641367 ),
    vec2(  0.14383161,  -0.14100790 )
);


float shadowmapSample (vec3 shadowmapVert, vec2 disp, float bias)
{
    return texture (uTxtShadowMap, vec3 (shadowmapVert.xy + disp, shadowmapVert.z - bias));
}


float shadowmapShading (float normalXlight2Vert)
{
    vec3 shadowmapVert = vShadowmapVert.xyz / vShadowmapVert.w;

    float acneBias = 0.0008 * tan (acos (normalXlight2Vert));
    acneBias = clamp (acneBias, 0.000001, 0.01);
    float blurStrength = 1.0 / 3500.0;

    int softenRingsNum = 2;
    int softenRingSamplesNum = 4;

    float shadow = 0;
    float dispScale = blurStrength * softenRingsNum;
    shadow += shadowmapSample (shadowmapVert, vec2 ( 1,  1) * dispScale, acneBias);
    shadow += shadowmapSample (shadowmapVert, vec2 (-1, -1) * dispScale, acneBias);
    shadow += shadowmapSample (shadowmapVert, vec2 (-1,  1) * dispScale, acneBias);
    shadow += shadowmapSample (shadowmapVert, vec2 ( 1, -1) * dispScale, acneBias);
    shadow /= 4;

    if (shadow < 1)
    {
        for (int j = 1; j <= softenRingsNum; j++)
            for (int i = 0; i < softenRingSamplesNum; i++)
            {
                float blurredProbe = blurStrength * (j);
                int poissonIndex = (j - 1) * softenRingSamplesNum + i;
                shadow += shadowmapSample (shadowmapVert, cPoissonDistribution16[poissonIndex] * blurredProbe, acneBias);
            }

        shadow /= softenRingSamplesNum * softenRingsNum + 1;
    }

    float shadowLightenFactor = 0.15;
    shadow = shadowLightenFactor + shadow * (1 - shadowLightenFactor);

    return shadow;
}


void main()
{
    vec3 normal     = normalize (vNormal);
    vec3 light2Vert = normalize (vLight2Vert);
    vec3 vert2Eye   = normalize (vVert2Eye);

    float normalXlight2Vert = dot (normal, light2Vert);
    vec3 diffColor = texture2D (uTxtDiffuse, vTexUV).xyz;
    float diffLightness = max (0, normalXlight2Vert);
    float ambientLightness = 0.2;

    float shadow = shadowmapShading (normalXlight2Vert);

    vec3 totalColor = uLightColor * diffColor;
    float totalLightness = diffLightness * shadow * (1 - ambientLightness) + ambientLightness;
    oColor = totalLightness * totalColor;

    oNormal = vViewSpaceNormal / 2 + 0.5;
}
