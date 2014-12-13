
#version 330 core

//----------------------------------------------------------------------------------------------------------------------

uniform sampler2D  uTxtInput;
uniform vec2  uFrameSize;

in vec2  vTexUV;
out vec3  oColor;

//----------------------------------------------------------------------------------------------------------------------

float FXAA_SPAN_MAX   = 10.0;
float FXAA_REDUCE_MUL = 1.0 / 16.0;
float FXAA_REDUCE_MIN = 1.0 / 64.0;

//----------------------------------------------------------------------------------------------------------------------

void main()
{
    vec2 screenUV = vTexUV + vec2 (0.5, 0.5) / uFrameSize;
    vec3 rgbNW = texture (uTxtInput, screenUV + (vec2 (-1.0, -1.0) / uFrameSize)).xyz;
    vec3 rgbNE = texture (uTxtInput, screenUV + (vec2 (+1.0, -1.0) / uFrameSize)).xyz;
    vec3 rgbSW = texture (uTxtInput, screenUV + (vec2 (-1.0, +1.0) / uFrameSize)).xyz;
    vec3 rgbSE = texture (uTxtInput, screenUV + (vec2 (+1.0, +1.0) / uFrameSize)).xyz;
    vec3 rgbM = texture (uTxtInput, screenUV).xyz;

    vec3 luma = vec3 (0.299, 0.587, 0.114);
    float lumaNW = dot (rgbNW, luma);
    float lumaNE = dot (rgbNE, luma);
    float lumaSW = dot (rgbSW, luma);
    float lumaSE = dot (rgbSE, luma);
    float lumaM = dot (rgbM, luma);

    float lumaMin = min (lumaM, min (min (lumaNW, lumaNE), min (lumaSW, lumaSE)));
    float lumaMax = max (lumaM, max (max (lumaNW, lumaNE), max (lumaSW, lumaSE)));

    vec2 dir;
    dir.x = - ((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max ((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);

    float rcpDirMin = 1.0 / (min (abs (dir.x), abs (dir.y)) + dirReduce);

    dir = min (vec2 (FXAA_SPAN_MAX, FXAA_SPAN_MAX),
              max (vec2 (-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) / uFrameSize;

    vec3 rgbA = (1.0 / 2.0) * (
            texture (uTxtInput, vTexUV + dir * (1.0/3.0 - 0.5)).xyz +
            texture (uTxtInput, vTexUV + dir * (2.0/3.0 - 0.5)).xyz);

    vec3 rgbB = rgbA * (1.0 / 2.0) + (1.0 / 4.0) * (
            texture (uTxtInput, vTexUV + dir * (0.0 - 0.5)).xyz +
            texture (uTxtInput, vTexUV + dir * (3.0/3.0 - 0.5)).xyz);
    float lumaB = dot (rgbB, luma);

    if ((lumaB < lumaMin) || (lumaB > lumaMax))  oColor = rgbA;
    else                                         oColor = rgbB;
}
