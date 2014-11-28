
#version 330 core

uniform sampler2D  uNormalMap;
uniform sampler2D  uRefractionDepth;
uniform sampler2D  uRefraction;
uniform sampler2D  uReflection;

uniform vec2  uClipNearFar;
uniform float  uFrameCount;
uniform vec3 uLightColor;


in vec2  vTexUV;
in vec3  vLight2VertPos;
in vec3  vVert2Eye;
in vec4  proj_coords;
in vec3  vVertWorld;

out vec4  out_Color;

//----------------------------------------------------------------------------------------------------------------------

vec3 waterColor = vec3 (0.5, 0.8, 0.8);
//const vec3 waterSpecular = vec3 (0.9, 0.9, 1);

const float diffuseLightenFactor = 0.1;
const float density = 0.45;
const float coastDensity = 0.97;
const float refractionDistort = 0.33;
const float reflectionDistort = 0.055;
const float specularity = 950;

//----------------------------------------------------------------------------------------------------------------------

float calculateLinearDepth (float zScreen)
{
    float zFar = uClipNearFar.y;
    float zNear = uClipNearFar.x;

    return zFar * zNear / (zFar - zScreen * (zFar - zNear));
}


vec3 calculatePixelNormal()
{
    vec2 texUV = vTexUV + vec2 (uFrameCount);

    vec3 normalPixel =       texture (uNormalMap, vTexUV).xzy +
                             texture (uNormalMap, -texUV / 3).xzy +
                       1.5 * texture (uNormalMap, vTexUV / 20).xzy +
                       0.5 * texture (uNormalMap, vec2 (texUV.x, vTexUV.y) * 3).xzy;
    normalPixel /= 4.0;

    return normalize (normalPixel * 2.0 + vec3 (-1, 0.3 - 1, -1));
}


void main()
{
    vec3 vert2Eye   = normalize (vVert2Eye);
    vec3 light2Vert = normalize (vLight2VertPos);
    vec3 pixelNormal = calculatePixelNormal();

    vec3 waterSpecular = uLightColor * 2.1;
    waterColor *= uLightColor;

    float diffuseLight = clamp (dot (pixelNormal, light2Vert), 0, 1);
    diffuseLight = diffuseLight * (1 - diffuseLightenFactor) + diffuseLightenFactor;

    vec3 light2VertReflected = reflect (-light2Vert, pixelNormal);
    // a^b = a / (b – a*b + a)
    float specBase = max (dot (vert2Eye, light2VertReflected), 0.0);
    vec3 specular = waterSpecular * (specBase / (specBase + specularity - specBase*specularity));


    vec3 screenVert  = 0.5 * proj_coords.xyz / proj_coords.w + 0.5;
    float cameraDepth = calculateLinearDepth (screenVert.z);
    float refractionDepth = calculateLinearDepth (texture (uRefractionDepth, screenVert.xy).x);
    float waterDepth = refractionDepth - cameraDepth;

    vec2 expWaterDepth = vec2 (density, coastDensity);
    expWaterDepth *= waterDepth;
    expWaterDepth = vec2 (1.0) - exp (-expWaterDepth);

    vec2 distortVec = pixelNormal.zx * refractionDistort * expWaterDepth.x;
    float distortedRefractDepth = calculateLinearDepth (texture (uRefractionDepth, screenVert.xy + distortVec).x);
    float expDistortedDepth = expWaterDepth.x;

    if (cameraDepth > distortedRefractDepth)  distortVec = vec2 (0.0);
    else  expDistortedDepth = 1.0 - exp (-density * (distortedRefractDepth - cameraDepth));

    vec3 refractionColor = texture (uRefraction, screenVert.xy + distortVec).xyz;
    refractionColor = mix (refractionColor, waterColor, expDistortedDepth);


    distortVec = pixelNormal.xz * reflectionDistort;
    vec3 reflectionColor = texture (uReflection, screenVert.xy + distortVec).xyz;

    float fresnelCoeff = 1.0 - dot (vert2Eye, pixelNormal);
    float reflectionMixFactor = fresnelCoeff * expWaterDepth.y * 0.85;
    vec3 resultColor = mix (refractionColor, reflectionColor, reflectionMixFactor);

    resultColor *= mix (1, diffuseLight, expWaterDepth.y);
    resultColor += specular;

    float fogFactor = sqrt (dot (vVertWorld, vVertWorld) * length (vVertWorld)) / 10000;
    resultColor = mix (resultColor, (vec3 (0.1, 0.1, 0.1) + uLightColor), fogFactor);

    //resultColor /= 2;
    out_Color = vec4 (resultColor, 1.0 - fogFactor*fogFactor * 4);
}
