
#version 330 core

#with  ../common/linear_depth.glsl

//----------------------------------------------------------------------------------------------------------------------

uniform sampler2D  uTxtDepthMap;

uniform vec2  uFrameSize;
uniform vec2  uClipNearFar;

//----------------------------------------------------------------------------------------------------------------------

uniform float  uDensity = 0.17;
uniform vec3   uColor   = vec3 (1, 1, 1);

out vec4  oColor;

//----------------------------------------------------------------------------------------------------------------------

void main()
{
    float originalZ = texture (uTxtDepthMap, gl_FragCoord.xy / uFrameSize).r;
    float originDepth = calculateLinearDepth (uClipNearFar, originalZ);
    float fogDepth = calculateLinearDepth (uClipNearFar, gl_FragCoord.z);

    float deltaDepth = originDepth - fogDepth;

    float fogFactor = deltaDepth * uDensity;
    fogFactor *= sqrt (fogFactor);
    fogFactor = clamp (fogFactor, 0, 0.85);

    oColor = vec4 (uColor, fogFactor);
}
