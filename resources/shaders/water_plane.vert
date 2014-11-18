
#version 130

in vec3 aCoords;
in vec2 aTexUV;

uniform mat4 uMatTransform;
uniform mat4 uMatWorldTransform;
uniform vec3 uLightPos;
uniform vec3 uViewPos;

out vec3 vVert2Eye;
out vec3 vVertWorld;
out vec2 vTexUV;
out vec3 vLight2VertPos;
out vec4 proj_coords;


void main()
{
    gl_Position = uMatTransform * vec4 (aCoords, 1);
    vTexUV = aTexUV * 200;

    vec4 spaceVertex4 = uMatWorldTransform  * vec4 (aCoords, 1);
    vec3 spaceVertex = spaceVertex4.xyz;

    vLight2VertPos = uLightPos ;
    vVert2Eye = uViewPos - spaceVertex;
    proj_coords = gl_Position;
    vVertWorld = spaceVertex;
}
