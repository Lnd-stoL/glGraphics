
#version 330 core

uniform sampler2D  uDepthMap;
uniform sampler2D  uColorMap;

in vec4  vScreenUV;
out vec4  out_Color;


void main()
{
    vec3 proj = (vScreenUV.xyz / vScreenUV.w) * 0.5 + 0.5;
    float sceneDepth = texture (uDepthMap, proj.xy).r;
    vec4 backColor = vec4 (texture (uColorMap, proj.xy).rgb, 1);

    out_Color = vec4 (vec3 (1), 0.5);

    vec3 fogColor = vec3 (0.7, 0.7, 0.7);
    float fogFactor = pow ((1 - (proj.z - sceneDepth)), 10);
    out_Color = vec4 ((fogColor * fogFactor  + backColor.rgb) / (1 + fogFactor), 1);

    //if ( < proj.z)  out_Color = vec4 (texture (uColorMap, proj.xy).rgb, 1);
    //out_Color = vec4 (proj.xy, 0, 1);
}
