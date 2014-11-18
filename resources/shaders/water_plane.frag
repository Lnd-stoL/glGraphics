
#version 130

uniform sampler2D  uNormalMap;
uniform sampler2D  uRefractionDepth;
uniform sampler2D  uRefraction;
uniform sampler2D  uReflection;

uniform float uFrameCount;

varying vec2 vTexUV;
varying vec3 vLight2VertPos;
varying vec3 vVert2Eye;
varying vec4 proj_coords;
varying vec3 vVertWorld;


float calculate_linear_depth (float value)
{
    float Zfar = 1000;
    float Znear = 1;

    return Zfar * Znear / ( Zfar - value * (Zfar - Znear) );
}


void main()
{
    vec3 waterColor = vec3 (0.5, 0.8, 0.8);
    vec3 waterSpecular = vec3 (0.9, 0.9, 1);
    vec3 vert2Eye   = normalize (vVert2Eye);

    vec3 light2Vert = normalize (vLight2VertPos);
    vec2 texUV = vTexUV + vec2 (uFrameCount / 2);
    vec3 normalPixel = texture (uNormalMap, vTexUV).xzy +
            texture (uNormalMap, -texUV / 3).xzy +
            1.5 * texture (uNormalMap, vTexUV / 20).xzy +
            0.5 * texture (uNormalMap, vec2 (texUV.x, vTexUV.y) * 3).xzy;
    normalPixel /= 4;
    vec3 normal = normalize (normalPixel * 2 - vec3 (1) + vec3 (0, 0.3, 0));
    float diffuseLight = clamp (dot (normal, light2Vert), 0, 1) * 0.8 + 0.2;

    vec3 r = reflect (-light2Vert, normal);
    vec3 specular = waterSpecular * pow (max (dot (vert2Eye, r), 0.0), 400);


    float density = 0.8;
    vec3 proj_tc  = 0.5 * proj_coords.xyz / proj_coords.w + 0.5;
    float fresnel = 1.0 - dot (vert2Eye, normal);
    float fOwnDepth = calculate_linear_depth (proj_tc.z);
    float fSampledDepth = texture (uRefractionDepth, proj_tc.xy).x;
    fSampledDepth       = calculate_linear_depth (fSampledDepth);
    float fLinearDepth  = fSampledDepth - fOwnDepth;

    float fExpDepth = 1.0 - exp( -density * fLinearDepth);
    float fExpDepthHIGH = 1.0 - exp( -0.96 * fLinearDepth );

    float fDistortScale = 0.23 * fExpDepth;
    vec2 vDistort = normal.zx * fDistortScale;
    float fDistortedDepth = texture (uRefractionDepth, proj_tc.xy + vDistort).x;
    fDistortedDepth = calculate_linear_depth (fDistortedDepth);
    float fDistortedExpDepth =
            1.0 - exp( -density * (fDistortedDepth - fOwnDepth) );
    if (fOwnDepth > fDistortedDepth)
    {
        vDistort = vec2 (0.0);
        fDistortedExpDepth = fExpDepth;
    }
    vec3 refraction = texture (uRefraction, proj_tc.xy + vDistort).xyz;
    refraction = mix (refraction, waterColor * diffuseLight, fDistortedExpDepth);

    vDistort = normal.xz * 0.055;
    vec3 reflection = texture (uReflection, proj_tc.st + vDistort).xyz;
    float fMix = fresnel * fExpDepthHIGH * 0.9;
    vec3 outColor = mix (refraction, reflection, fMix);
    outColor *= mix (1, diffuseLight, fExpDepthHIGH * 0.9);
    outColor += specular;

    float fog = pow (length (vVertWorld), 1.5) / 10000;
    outColor = (outColor + vec3 (1, 1, 1) * fog) / (1 + fog);


    const float LOG2 = 1.442695;
    float z = length (vVert2Eye);
    float fogDensity = 0.005;
    float fogFactor = exp2 (-fogDensity*
                      fogDensity *
                      z *
                      z *
                      LOG2 );
    fogFactor = clamp(fogFactor, 0.0, 1.0) * 0.5 + 0.5;
    //outColor = mix (vec3 (0.5, 0.5, 0.5), outColor, fogFactor);

    gl_FragData[0] = vec4 (outColor, 1 - fog*fog * 4);
    //gl_FragData[0] = vec4 (1, 0, 0, 1);
}
