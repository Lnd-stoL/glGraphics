
#version 330 core

uniform sampler2D  uTexture;

in  vec2  gTexUV;
out vec4  oColor;

uniform vec3  uLightColor;


void main()
{
    vec4 texColor = texture (uTexture, gTexUV).rgba;
    texColor.rgb *= uLightColor;

    oColor = texColor;
    oColor.a /= 3;
    if (texColor.a < 0.1)  oColor.a = 0;
}
