
#version 330 core

uniform sampler2D  uFontBitmap;
uniform vec3  uColor;

in vec2 vTexUV;
out vec4 out_Color;


void main()
{
    vec4 letter = texture (uFontBitmap, vTexUV);
    out_Color = vec4 (uColor * letter.rgb, letter.a);
}
