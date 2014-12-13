
#version 330 core

uniform sampler2D  uDepthMap;

uniform vec2  uFrameSize = vec2 (1400, 1100);
uniform vec2  uClipNearFar;

uniform float  uDensity = 0.17;
uniform vec3   uColor   = vec3 (1, 1, 1);

out vec4  oColor;


float calculateLinearDepth (float zScreen)
{
    float zFar = uClipNearFar.y;
    float zNear = uClipNearFar.x;

    return zFar * zNear / (zFar - zScreen * (zFar - zNear));
}


void main()
{
    float originalZ = texture (uDepthMap, gl_FragCoord.xy / uFrameSize).r;
    float originDepth = calculateLinearDepth (originalZ);
    float fogDepth = calculateLinearDepth (gl_FragCoord.z);

    float deltaDepth = originDepth - fogDepth;
    //deltaDepth = clamp (deltaDepth, 0, 5);

    float fogFactor = deltaDepth * uDensity;
    fogFactor *= sqrt (fogFactor);
    fogFactor = clamp (fogFactor, 0, 0.85);

    oColor = vec4 (uColor, fogFactor);
}
