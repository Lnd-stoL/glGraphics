
#version 330 core

//----------------------------------------------------------------------------------------------------------------------

in vec2  iCoords;
in vec2  iTexUV;

out vec2 vTexUV;

uniform vec2  uOffset;
uniform vec2  uScale;

//----------------------------------------------------------------------------------------------------------------------

void main()
{
    gl_Position = vec4 (iCoords * uScale + uOffset, 0, 1);
    vTexUV = iTexUV;
}
