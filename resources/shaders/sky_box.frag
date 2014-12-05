
#version 330 core

//----------------------------------------------------------------------------------------------------------------------
// uniforms

uniform samplerCube  uSkyBox_Cubemap;
uniform sampler2D  uClouds;
uniform sampler3D  uClouds3D;
uniform float  uFrameCount;

uniform vec3 lightdir;
uniform vec3 uLightColor;

//----------------------------------------------------------------------------------------------------------------------
// input

in vec3  vTexCube;
out vec3  out_Color;

//----------------------------------------------------------------------------------------------------------------------
// global parameters

vec3 Kr = vec3 (0.48867780436772762, 0.6978442963618773, 0.7616065586417131);

const float rayleigh_brightness = 4.3;
const float mie_brightness      = 0.09;
const float spot_brightness     = 5;
const float scatter_strength    = 0.048;
const float rayleigh_strength   = 0.139; // 0.239
const float mie_strength        = 0.0264;

const float rayleigh_collection_power = 0.71;
const float mie_collection_power      = 0.39;
const float mie_distribution          = 0.63;

const float surface_height = 0.995;
const float intensity      = 1.9;
const int step_count = 8;

//----------------------------------------------------------------------------------------------------------------------

float atmospheric_depth (vec3 position, vec3 dir)
{
    float a = dot (dir, dir);
    float b = 2.0 * dot (dir, position);
    float c = dot (position, position) - 1.0;
    float detSqrt = sqrt (b*b - a*c*4);
    float q = (-b - detSqrt) / 2;

    return c / q;
}


float phase (float alpha, float g)
{
    float sq_g = g * g;
    float sq_alpha = alpha * alpha;

    float a = 3 * (1 - sq_g);
    float b = 2 * (2 + sq_g);
    float c = 1 + sq_alpha;
    float d = pow (1 + sq_g - 2*g*alpha, 1.5);

    return (a / b) * (c / d);
}


float horizon_extinction (vec3 position, vec3 dir, float radius)
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
    return color - color * pow (Kr, vec3 (factor / dist));
}


vec3 calcSkyColor (vec3 eyedir)
{
    float alpha = dot (eyedir, lightdir);

    float rayleigh_factor = phase (alpha, -0.01) * rayleigh_brightness;
    float mie_factor = phase (alpha, mie_distribution) * mie_brightness;
    float spot = smoothstep (0.0, 15.0, phase (alpha, 0.9995)) * spot_brightness;

    vec3 eye_position = vec3 (0.0, surface_height, 0.0);
    float eye_depth = atmospheric_depth (eye_position, eyedir);
    float step_length = eye_depth / float (step_count);
    float eye_extinction = horizon_extinction (eye_position, eyedir, surface_height - 0.15);

    vec3 rayleigh_collected = vec3 (0.0, 0.0, 0.0);
    vec3 mie_collected = vec3 (0.0, 0.0, 0.0);

    for (int i = 0; i < step_count; ++i)
    {
        float sample_distance = step_length * float(i);
        vec3 position = eye_position + eyedir * sample_distance;
        float extinction = horizon_extinction (position, lightdir, surface_height - 0.25);   // 0.35
        float sample_depth = atmospheric_depth (position, lightdir);
        vec3 influx = absorb (sample_depth, vec3 (intensity), scatter_strength) * extinction;

        rayleigh_collected += absorb (sample_distance, Kr * influx, rayleigh_strength);
        mie_collected += absorb (sample_distance, influx, mie_strength);
    }

    rayleigh_collected = (rayleigh_collected * eye_extinction * pow (eye_depth, rayleigh_collection_power)) / float (step_count);
    mie_collected = (mie_collected * eye_extinction * pow (eye_depth, mie_collection_power)) / float(step_count);

    return vec3 (spot*mie_collected + mie_factor*mie_collected + rayleigh_factor*rayleigh_collected);
}


vec4 sampleCloudsPlane (float frameTime, vec2 offset)
{
    float sphereProjHeight = vTexCube.y + 0.2;
    vec2 cloudsUV = vec2 ((vTexCube.x + offset.x) / sphereProjHeight / 5 + frameTime / 10,
                          (vTexCube.z + offset.y) / sphereProjHeight / 7 + frameTime / 5);

    float cint;
    cint = texture (uClouds3D, vec3 (cloudsUV, frameTime / 2)).r;
    //return texture (uClouds, cloudsUV);
    return vec4 (vec3 (cint), cint);
}


void main()
{
    if (vTexCube.y * 500 < -100)
    {
        out_Color = vec3 (1, 1, 1);
        return;
    }

    float frameTime = uFrameCount * 3;
    //lightdir = normalize (vec3 (cos (frameTime), sin (frameTime), 0.2));

    vec4 cloudColor = sampleCloudsPlane (frameTime, vec2 (0, 0)) * vec4 ((uLightColor + vec3 (1)) / 2, 1);

    float cloudTexturePixel = 0.01;
    float cloudNormalDX = length (sampleCloudsPlane (frameTime, vec2 (cloudTexturePixel, 0))) -
                            length (sampleCloudsPlane (frameTime, vec2 (-cloudTexturePixel, 0)));
    float cloudNormalDZ = length (sampleCloudsPlane (frameTime, vec2 (0, cloudTexturePixel))) -
                          length (sampleCloudsPlane (frameTime, vec2 (0, -cloudTexturePixel)));
    vec3 cloudNormal = normalize (vec3 (cloudNormalDX, 0.9, cloudNormalDZ));

    float cloudLight = clamp (dot (cloudNormal, lightdir /** 10000 - vec3 (cloudsUV.x * 500, 500, cloudsUV.y * 500)*/), 0, 1);
    cloudLight =  pow (cloudLight, 0.5);
    cloudLight = 1;

    vec3 skyColor = calcSkyColor (normalize (vTexCube));

    //color = mix (color, vec3 (1, 1, 1), 0.05);
    //cloudColor *= cloudLight;
    out_Color = mix (skyColor, cloudColor.rgb * (uLightColor + vec3 (0.2, 0.2, 0.2)), cloudColor.a * 0.96 * cloudLight);
    //out_Color = cloudColor.rgb;
}
