
#version 130

uniform sampler2D  uScreen;
uniform sampler2D  uNormalMap;
uniform sampler2D  uDepthMap;
uniform mat4 uMatInvProjection;

varying vec2 vTexUV;


float g_sample_rad = 0.1;
float g_intensity = 2.5;
float g_scale = 10;
float g_bias = 0.1;


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


float doAmbientOcclusion (vec2 tcoord, vec2 uv, vec3 p, vec3 cnorm)
{
    vec3 diff = calculatePosition (tcoord + uv, texture (uDepthMap, tcoord + uv).r * 2 - 1) - p;
    vec3 v = normalize (diff);
    float d = length (diff) * g_scale;
    return max (0.0, dot (cnorm, v) - g_bias) * (1.0 / (1.0 + d)) * g_intensity;
}


void main()
{
    vec3 screenOriginalColor = texture (uScreen, vTexUV).rgb;

    vec3 normal = normalize (texture (uNormalMap, vTexUV.xy).rgb * 2.0 - 1.0);
    float depth = texture (uDepthMap, vTexUV).r;
    vec3 viewPos = calculatePosition (vTexUV, depth * 2 - 1);

    if (viewPos.z < -100)
    {
        gl_FragData[0] = vec4 (screenOriginalColor, 1);
        return;
    }

    const vec2 vec[4] = vec2[]( vec2 (1, 0), vec2 (-1, 0), vec2 (0, 1), vec2 (0, -1) );

    vec2 rand = poisson16[int (length (normal) * 16)];

    float ao = 0.0f;
    float rad = g_sample_rad / viewPos.z;

//**SSAO Calculation**//
    int iterations = 4;
    for (int j = 0; j < iterations; ++j)
    {
        vec2 coord1 = reflect (vec[j], rand) * rad;
        vec2 coord2 = vec2 (coord1.x*0.707 - coord1.y*0.707,
                               coord1.x*0.707 + coord1.y*0.707);

        ao += doAmbientOcclusion (vTexUV, coord1*0.25, viewPos, normal);
        ao += doAmbientOcclusion (vTexUV, coord2*0.5, viewPos, normal);
        ao += doAmbientOcclusion (vTexUV, coord1*0.75, viewPos, normal);
        ao += doAmbientOcclusion (vTexUV, coord2, viewPos, normal);
    }
    ao /= float (iterations)*4.0;

    gl_FragData[0] = vec4 (screenOriginalColor * (1 - ao), 1);
    //gl_FragData[0] = vec4 (vec3 (1 - ao), 1);
}
