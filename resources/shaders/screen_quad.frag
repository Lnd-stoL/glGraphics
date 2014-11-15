
#version 130

uniform sampler2D  uScreen;

varying vec2 vTexUV;


void main()
{
    gl_FragData[0] = texture (uScreen, vTexUV);
}
