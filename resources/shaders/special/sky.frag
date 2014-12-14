
#version 330 core

//----------------------------------------------------------------------------------------------------------------------

uniform sampler3D  uTxtClouds3D;
uniform float  uDayTime;

uniform vec3  uSunDir;
uniform vec3  uSunColor;
uniform float uSkySphereRadius;

//----------------------------------------------------------------------------------------------------------------------

in  vec3  vTexUVTCube;
out vec3  oColor;

//----------------------------------------------------------------------------------------------------------------------
// tuning parameters

vec3 _skyColorKr = vec3 (0.48867780436772762, 0.6978442963618773, 0.7616065586417131);

const float _rayleighBrightness = 4.3;
const float _mieBrightness      = 0.09;
const float _spotBrightness     = 5;
const float _scatterStrength    = 0.048;
const float _rayleighStrength   = 0.139;  // 0.239
const float _mieStrength        = 0.0264;

const float _rayleighCollectionPower = 0.71;
const float _mieCollectionPower      = 0.39;
const float _mieDistribution         = 0.63;

const float _surfaceHeight   = 0.995;
const float _intensityLight  = 1.9;
const int   _rayTracingSteps = 10;

//----------------------------------------------------------------------------------------------------------------------

float calcAtmosphericDepth (vec3 position, vec3 dir)
{
    float a = dot (dir, dir);
    float b = 2.0 * dot (dir, position);
    float c = dot (position, position) - 1.0;
    float detSqrt = sqrt (b*b - a*c*4);
    float q = (-b - detSqrt) / 2.0;

    return c / q;
}


float calcPhase (float alpha, float g)
{
    float sq_g     = g * g;
    float sq_alpha = alpha * alpha;

    float a = 3 * (1 - sq_g);
    float b = 2 * (2 + sq_g);
    float c = 1 + sq_alpha;
    float d = pow (1 + sq_g - 2*g*alpha, 1.5);

    return (a / b) * (c / d);
}


float calcHorizonExtinction (vec3 position, vec3 dir, float radius)
{
    float u = dot (dir, -position);
    if (u < 0)  return 1.0;

    vec3 near = position + u*dir;
    if (length (near) < radius)  return 0.0;
    else
    {
        vec3 v2 = normalize (near) * radius - position;
        float diff = acos (dot (normalize (v2), dir));
        return smoothstep (0.0, 1.0, pow (diff*2, 3));
    }
}


vec3 absorb (float dist, vec3 color, float factor)
{
    return color - color * pow (_skyColorKr, vec3 (factor / dist));
}


vec3 calcSkyColor (vec3 rayDir)
{
    float alpha = dot (rayDir, uSunDir);

    float rayleighFactor = calcPhase (alpha, -0.01) * _rayleighBrightness;
    float mieFactor = calcPhase (alpha, _mieDistribution) * _mieBrightness;
    float spot = smoothstep (0, 15.0, calcPhase (alpha, 0.9995)) * _spotBrightness;

    vec3 eyePosition = vec3 (0, _surfaceHeight, 0);
    float eyeDepth = calcAtmosphericDepth (eyePosition, rayDir);
    float fStepCount = float (_rayTracingSteps);
    float stepLength = eyeDepth / fStepCount;
    float eyeExtinction = calcHorizonExtinction (eyePosition, rayDir, _surfaceHeight - 0.15);

    vec3 rayleighCollected = vec3 (0.0, 0.0, 0.0);
    vec3 mieCollected      = vec3 (0.0, 0.0, 0.0);

    for (int i = 0; i < _rayTracingSteps; ++i)
    {
        float sampleDistance = stepLength * float (i);
        vec3 position = eyePosition + rayDir * sampleDistance;

        float extinction   = calcHorizonExtinction (position, uSunDir, _surfaceHeight - 0.35);
        float sampleDepth  = calcAtmosphericDepth (position, uSunDir);
        vec3 influx = absorb (sampleDepth, vec3 (_intensityLight), _scatterStrength) * extinction;

        rayleighCollected += absorb (sampleDistance, _skyColorKr * influx, _rayleighStrength);
        mieCollected      += absorb (sampleDistance, influx, _mieStrength);
    }

    rayleighCollected = (rayleighCollected * eyeExtinction * pow (eyeDepth, _rayleighCollectionPower)) / fStepCount;
    mieCollected      = (mieCollected * eyeExtinction * pow (eyeDepth, _mieCollectionPower)) / fStepCount;

    return vec3 (spot * mieCollected + mieFactor * mieCollected + rayleighFactor * rayleighCollected);
}


float sampleCloudsPlane (float frameTime, vec2 offset)
{
    float sphereProjHeight = vTexUVTCube.y + 0.25;
    vec2 cloudsUV = vec2 ((vTexUVTCube.x + vTexUVTCube.x) / sphereProjHeight / 6 + frameTime / 5,
                          (vTexUVTCube.z + vTexUVTCube.y) / sphereProjHeight / 3 + frameTime / 8);

    return texture (uTxtClouds3D, vec3 (cloudsUV, frameTime / 3)).x * smoothstep (0, 1, sphereProjHeight);
}


void main()
{
    if (vTexUVTCube.y * uSkySphereRadius < -uSkySphereRadius / 5)    { discard; return; }   // under water

    vec3 cloudNormal = vec3 (0, -1, 0);
    float cloudLightness = dot (cloudNormal, -uSunDir) + 0.3;
    cloudLightness = clamp (cloudLightness, 0, 1);

    float animationTime = uDayTime * 3;
    float cloudColor = sampleCloudsPlane (animationTime, vec2 (0, 0));

    vec3 skyColor = calcSkyColor (normalize (vTexUVTCube));
    oColor = mix (skyColor, (mix (uSunColor, vec3(1), 0.3) * cloudLightness), cloudColor * 0.95);
}
