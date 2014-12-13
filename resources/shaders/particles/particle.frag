
#version 330 core

uniform sampler2D  uTexture;

in  vec2  gTexUV;
out vec4  oColor;


void main()
{
    vec4 texColor = texture (uTexture, gTexUV).rgba;
    oColor = texColor;
    if (texColor.a < 0.1)  oColor.a = 0;
}
