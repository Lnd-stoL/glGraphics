
#with  poisson.glsl

//----------------------------------------------------------------------------------------------------------------------

float _shadowmapSample (sampler2DShadow txtShadowMap, vec3 shadowmapVert, vec2 disp, float bias)
{
    return texture (txtShadowMap, vec3 (shadowmapVert.xy + disp, shadowmapVert.z - bias));
}


float shadowmapShading (vec4 v_shadowmapVert, sampler2DShadow txtShadowMap, float normalXlight2Vert)
{
    vec3 shadowmapVert = v_shadowmapVert.xyz / v_shadowmapVert.w;

    float acneBias = 0.003 * tan (acos (normalXlight2Vert));
    acneBias = clamp (acneBias, 0.000001, 0.01);
    float blurStrength = 1.0 / 3200.0;

    int softenRingsNum = 3;
    int softenRingSamplesNum = 4;

    float shadow = 0;
    float dispScale = blurStrength * softenRingsNum;
    shadow += _shadowmapSample (txtShadowMap, shadowmapVert, vec2 ( 1,  1) * dispScale, acneBias);
    shadow += _shadowmapSample (txtShadowMap, shadowmapVert, vec2 (-1, -1) * dispScale, acneBias);
    shadow += _shadowmapSample (txtShadowMap, shadowmapVert, vec2 (-1,  1) * dispScale, acneBias);
    shadow += _shadowmapSample (txtShadowMap, shadowmapVert, vec2 ( 1, -1) * dispScale, acneBias);
    shadow /= 4;

    if (shadow < 1)
    {
        for (int j = 1; j <= softenRingsNum; j++)
            for (int i = 0; i < softenRingSamplesNum; i++)
            {
                float blurredProbe = blurStrength * (j);
                int poissonIndex = (j - 1) * softenRingSamplesNum + i;
                shadow += _shadowmapSample (txtShadowMap, shadowmapVert, cPoissonDistribution16[poissonIndex] * blurredProbe, acneBias);
            }

        shadow /= softenRingSamplesNum * softenRingsNum + 1;
    }

    float shadowLightenFactor = 0.15;
    shadow = shadowLightenFactor + shadow * (1 - shadowLightenFactor);

    return shadow;
}
