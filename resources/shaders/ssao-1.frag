
#version 330 core

uniform sampler2D  uScreen;
uniform sampler2D  uNormalMap;
uniform sampler2D  uDepthMap;
uniform mat4 uMatInvProjection;

in vec2 vTexUV;
out vec3 out_Color;

const int sample_count = 16;
const vec2 poisson16[] = vec2[](    // These are the Poisson Disk Samples
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


vec3 calculatePosition (in vec2 coord, in float depth)
{
    vec2 screenCoords = coord * 2 - 1;
    vec3 coords = vec3 (screenCoords, depth);
    vec4 viewCoords = uMatInvProjection * vec4 (coords, 1);
    viewCoords.xyz /= viewCoords.w;
    //viewCoords.z = -viewCoords.z;

    return viewCoords.xyz;
}


vec3 normal_decode (vec2 enc)
{
    vec2 fenc = enc * 4 - 2;
    float f = dot (fenc,fenc);
    float g = sqrt (1 - f/4);
    vec3 n;
    n.xy = fenc*g;
    n.z = 1 - f/2;
    return n;
}


void main()
{
    vec3 screenOriginalColor = texture (uScreen, vTexUV).rgb;
    out_Color = screenOriginalColor;
    //return;

    vec3 normal = normalize (normal_decode (texture (uNormalMap, vTexUV.xy).xy));
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
        vec2 filterRadius = vec2 (0.06, 0.06);

        float ambientOcclusion = 0;
        float normSumm = 0;
        vec3 avgSamleDir = vec3 (0, 0, 0);
        for (int i = 0; i < 16; ++i)
        {
            vec2 disp = (filterRadius / viewPos.z);
            vec2 sampleTexCoord = vTexUV + (poisson16[i] * disp);
            float sampleDepth = texture (uDepthMap, sampleTexCoord).r;
            vec3 samplePos = calculatePosition (sampleTexCoord, sampleDepth * 2 - 1);
            vec3 sampleDir = normalize (samplePos - viewPos);

            float NdotS = max (dot (normal, sampleDir), -1);
            float VPdistSP = distance (viewPos, samplePos);

            float a = 1.0 - smoothstep (0.1, 0.2, VPdistSP);
            float b = NdotS;

            float d = 0.000001;
            float c = smoothstep (d, 10000*d, VPdistSP);
            //if (VPdistSP >= 0.02)  c = 0;
            if (VPdistSP > 0.5) VPdistSP = 0;
            float norm = length (disp);
            if (NdotS > 0)
            {
                ambientOcclusion += 1 - (VPdistSP) /*(NdotS)*/;
                normSumm += (1);
            }

            avgSamleDir += sampleDir;
        }

        ambientOcclusion = max (ambientOcclusion, 0.01);

        avgSamleDir /= normSumm;
        ambientOcclusion /= normSumm;
        //ambientOcclusion = clamp (ambientOcclusion, 0, 0.9);
        float occlusionFactor = 1.0 - (ambientOcclusion);

        ambientOcclusion = min (ambientOcclusion, 1);
        out_Color = screenOriginalColor * ambientOcclusion * ambientOcclusion * ambientOcclusion;
        //out_Color = vec3 (viewPos.xy, -viewPos.z);
        //out_Color = vec3 (ambientOcclusion);
        //out_Color = avgSamleDir;
        //gl_FragData[0] = vec4 (vec3 (occlusionFactor), 1);
        //gl_FragData[0] = vec4 (texture (uScreen, vTexUV).rgb, 1);
    }

}
