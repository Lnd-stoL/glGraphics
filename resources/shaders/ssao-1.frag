
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
    //screenCoords.y = -screenCoords.y;
    vec3 coords = vec3 (screenCoords, depth);
    vec4 viewCoords = uMatInvProjection * vec4 (coords, 1);
    viewCoords.xyz /= viewCoords.w;

    return viewCoords.xyz;
}


void main()
{
    //out_Color = vec4 (1, 0, 0, 1);
    //return;

    vec3 screenOriginalColor = texture (uScreen, vTexUV).rgb;
    //gl_FragData[0] = vec4 (screenOriginalColor, 1);
    //return;

    vec3 normal = normalize (texture (uNormalMap, vTexUV.xy).rgb * 2.0 - 1.0);
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
        vec2 filterRadius = vec2 (0.008, 0.006);

        float ambientOcclusion = 0;
        // perform AO
        for (int i = 0; i < sample_count; ++i)
        {
            // sample at an offset specified by the current Poisson-Disk sample and scale it by a radius (has to be in Texture-Space)
            vec2 sampleTexCoord = vTexUV + (poisson16[i] * (filterRadius));
            float sampleDepth = texture (uDepthMap, sampleTexCoord).r;
            vec3 samplePos = calculatePosition (sampleTexCoord, sampleDepth * 2 - 1);
            vec3 sampleDir = normalize (samplePos - viewPos);

            // angle between SURFACE-NORMAL and SAMPLE-DIRECTION (vector from SURFACE-POSITION to SAMPLE-POSITION)
            float NdotS = max (dot (normal, sampleDir), 0);
            // distance between SURFACE-POSITION and SAMPLE-POSITION
            float VPdistSP = distance (viewPos, samplePos);

            // a = distance function
            float a = 1.0 - smoothstep (distanceThreshold, distanceThreshold * 10, VPdistSP);
            // b = dot-Product
            float b = NdotS;

            ambientOcclusion += (a * b);
        }

        ambientOcclusion /= sample_count;
        //ambientOcclusion *= ambientOcclusion - 0.1;
        ambientOcclusion = clamp (ambientOcclusion, 0, 0.9);
        float occlusionFactor = 1.0 - (ambientOcclusion);

        out_Color = screenOriginalColor * occlusionFactor;
        //gl_FragData[0] = vec4 (vec3 (occlusionFactor), 1);
        //gl_FragData[0] = vec4 (texture (uScreen, vTexUV).rgb, 1);
    }

}
