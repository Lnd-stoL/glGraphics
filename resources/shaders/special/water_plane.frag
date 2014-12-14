
#version 330 core

//----------------------------------------------------------------------------------------------------------------------

uniform sampler2D  uNormalMap;
uniform sampler2D  uRefractionDepth;
uniform sampler2D  uRefraction;
uniform sampler2D  uReflection;

uniform vec2   uClipNearFar;
uniform float  uFrameCount;
uniform vec3   uLightColor;

//----------------------------------------------------------------------------------------------------------------------

in vec2  vTexUV;
in vec3  vLight2VertPos;
in vec3  vVert2Eye;
in vec4  proj_coords;
in vec3  vVertWorld;

//----------------------------------------------------------------------------------------------------------------------

out vec4  oColor;

//----------------------------------------------------------------------------------------------------------------------

vec3 waterColor = vec3 (0.5, 0.8, 0.8);

const float diffuseLightenFactor = 0.25;
const float density = 0.32;
const float coastDensity = 0.84;
const float refractionDistort = 0.22;
const float reflectionDistort = 0.075;
const float specularity = 980;

//----------------------------------------------------------------------------------------------------------------------

float calculateLinearDepth (float zScreen)
{
    float zFar = uClipNearFar.y;
    float zNear = uClipNearFar.x;

    return zFar * zNear / (zFar - zScreen * (zFar - zNear));
}


vec3 calculatePixelNormal()
{
    vec2 texUV = vTexUV + vec2 (uFrameCount * 1.7);

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

    vec3 waterSpecular = uLightColor * 1.9;
    waterColor *= uLightColor;

    float diffuseLight = clamp (dot (pixelNormal, light2Vert), 0, 1);
    diffuseLight = diffuseLight * (1 - diffuseLightenFactor) + diffuseLightenFactor;

    vec3 light2VertReflected = reflect (-light2Vert, pixelNormal);
    // a^b = a / (b – a*b + a)
    float specBase = max (dot (vert2Eye, light2VertReflected), 0.0);
    vec3 specular = waterSpecular * (specBase / (specBase + specularity - specBase * specularity));

    vec3 screenVert  = 0.5 * proj_coords.xyz / proj_coords.w + 0.5;
    float cameraDepth = calculateLinearDepth (screenVert.z);
    float refractionDepth = calculateLinearDepth (texture (uRefractionDepth, screenVert.xy).x);
    float waterDepth = refractionDepth - cameraDepth;

    vec2 expWaterDepth = vec2 (density, coastDensity);
    expWaterDepth *= waterDepth;
    expWaterDepth = vec2 (1.0) - exp (-expWaterDepth / 1.5);

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
    float reflectionMixFactor = fresnelCoeff * expWaterDepth.y * 0.85 + 0.1;
    vec3 resultColor = mix (refractionColor, reflectionColor, reflectionMixFactor);

    resultColor *= mix (1, diffuseLight, expWaterDepth.y - 0.38);
    resultColor += specular;

    float fogFactor = sqrt (dot (vVertWorld, vVertWorld) * length (vVertWorld)) / 10000;
    resultColor = mix (resultColor, (vec3 (0.1, 0.1, 0.1) + uLightColor), fogFactor);

    float  waterAlpha = (1.0 - fogFactor*fogFactor * 4) * 2;
    oColor = vec4 (resultColor, waterAlpha);
}
