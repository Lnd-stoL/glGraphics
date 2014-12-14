
#version 330 core

#with  ../common/poisson.glsl

//----------------------------------------------------------------------------------------------------------------------

uniform sampler2D  uScreen;
uniform sampler2D  uNormalMap;
uniform sampler2D  uDepthMap;

uniform vec2  uFrameSize;
uniform mat4 uMatInvProjection;

//----------------------------------------------------------------------------------------------------------------------

in vec2 vTexUV;
out vec3 out_Color;

//----------------------------------------------------------------------------------------------------------------------

vec3 calculatePosition (in vec2 coord, in float depth)
{
    vec2 screenCoords = coord * 2 - 1;
    vec3 coords = vec3 (screenCoords, depth);
    vec4 viewCoords = uMatInvProjection * vec4 (coords, 1);
    viewCoords.xyz /= viewCoords.w;

    return viewCoords.xyz;
}


void main()
{
    vec3 screenOriginalColor = texture (uScreen, vTexUV).rgb;

    vec3 normal = normalize (texture (uNormalMap, vTexUV.xy).xyz * 2 - 1);
    float depth = texture (uDepthMap, vTexUV).r;
    vec3 viewPos = calculatePosition (vTexUV, depth * 2 - 1);

    if (viewPos.z < -200)
    {
        out_Color = screenOriginalColor;
        return;
    }

    else
    {
        float distanceThreshold = 0.05;
        vec2 filterRadius = vec2 (3) / uFrameSize;

        float ambientOcclusion = 0;
        float normSumm = 0;
        vec3 avgSamleDir = vec3 (0, 0, 0);

        for (int i = 0; i < 16; ++i)
        {
            vec2 disp = (filterRadius / viewPos.z);
            vec2 sampleTexCoord = vTexUV + (cPoissonDistribution16[i] * disp);
            float sampleDepth = texture (uDepthMap, sampleTexCoord).r;
            vec3 samplePos = calculatePosition (sampleTexCoord, sampleDepth * 2 - 1);
            vec3 sampleDir = normalize (samplePos - viewPos);

            float NdotS = max (dot (normal, sampleDir), -1);
            float VPdistSP = distance (viewPos, samplePos);

            float a = 1.0 - smoothstep (0.1, 0.2, VPdistSP);
            float b = NdotS;

            float d = 0.000001;
            float c = smoothstep (d, 10000*d, VPdistSP);
            if (VPdistSP > 0.5) VPdistSP = 0;
            float norm = length (disp);
            if (NdotS > -0.51)
            {
                ambientOcclusion += 1 - (VPdistSP) /*(NdotS)*/;
                normSumm += (1);
            }

            avgSamleDir += sampleDir;
        }

        ambientOcclusion = max (ambientOcclusion, 0.05);

        avgSamleDir /= normSumm;
        ambientOcclusion /= normSumm;
        float occlusionFactor = 1.0 - (ambientOcclusion);

        ambientOcclusion = min (ambientOcclusion, 1);
        out_Color = screenOriginalColor * ambientOcclusion * ambientOcclusion * ambientOcclusion;
    }

}
