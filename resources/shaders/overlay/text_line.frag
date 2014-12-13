
#version 330 core

//----------------------------------------------------------------------------------------------------------------------

uniform sampler2D  uTxtFontBitmap;
uniform vec3  uColor;

in vec2 vTexUV;
out vec4 oColor;

//----------------------------------------------------------------------------------------------------------------------

void main()
{
    vec4 letter = texture (uTxtFontBitmap, vTexUV);
    oColor = vec4 (uColor * letter.rgb, letter.a);
}
