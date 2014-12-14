
#version 330 core

//----------------------------------------------------------------------------------------------------------------------

uniform sampler2D  uTxtInput;
uniform vec2       uFrameSize;

//----------------------------------------------------------------------------------------------------------------------

uniform float  uGamma      = 0.92;
uniform float  uBrightness = 1.01;
uniform float  uContrast   = 1.05;
uniform float  uSaturation = 0.15;

//----------------------------------------------------------------------------------------------------------------------

in vec2   vTexUV;
out vec3  oColor;

//----------------------------------------------------------------------------------------------------------------------

#with  fxaa.frag

//----------------------------------------------------------------------------------------------------------------------

vec3 desaturate (vec3 color, float amount)
{
    vec3 gray = vec3 (dot (vec3 (0.2126, 0.7152, 0.0722), color));
    return vec3 (mix (color, gray, amount));
}


void main()
{
    vec3 color = applyFxaa();

    float darkFrameDist = (length ((gl_FragCoord.xy / uFrameSize) - vec2 (0.5, 0.5)));
    //darkFrameDist = 1 - darkFrameDist;
    darkFrameDist = clamp (darkFrameDist, 0, 1);
    float darkFrame = darkFrameDist * 0.5 + 0.5;

    oColor =  mix (pow (color, vec3 (uGamma)) * uContrast, vec3 (1), uBrightness - 1);
    oColor = desaturate (oColor, -uSaturation);
}
