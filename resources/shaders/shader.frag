
#version 130

uniform sampler2D uTexture;
uniform sampler2DShadow uShadowMap;
uniform sampler2D uShadowMapFlat;

varying vec3 vNormal;
varying vec3 vLight2VertPos;
varying vec2 vTexUV;
varying vec3 vVert2Eye;
varying vec4 vShadowmapVert;


float shadowmapShading (vec3 vsv, vec2 disp, float bias)
{
    //float shadow = 1;
    //float shadowMapVal = texture (uShadowMap, (vsv.xy / 2 + vec2 (0.5, 0.5) + disp)).x;
    //if (shadowMapVal + 0.00003 < (vsv.z * 0.5 + 0.5))  shadow = 0;

    return texture (uShadowMap, vec3 (vsv.xy + disp, vsv.z - bias));
    //return texture (uShadowMap, vec3 (vsv.xy / 2 + vec2 (0.5, 0.5) + disp, vsv.z * 0.5 + 0.5 - 0.0001));

    //return shadow;
}


vec2 poissonDisk[4] = vec2[](
        vec2( -0.94201624, -0.39906216 ),
        vec2( 0.94558609, -0.76890725 ),
        vec2( -0.094184101, -0.92938870 ),
        vec2( 0.34495938, 0.29387760 )
);


void main()
{
    vec3 normal     = normalize (vNormal);
    vec3 light2Vert = normalize (vLight2VertPos);
    vec3 vert2Eye   = normalize (vVert2Eye);

    vec3 diff = texture2D (uTexture, vTexUV).xyz * max (0.1, dot (normal, light2Vert));

    float bias = 0.00005 * tan (acos (dot (normal, light2Vert)));
    bias = clamp (bias, 0.000001, 0.0001);

    vec3 vsv = vShadowmapVert.xyz / vShadowmapVert.w;

    float shadow = 0;
    for (int i = 0; i < 4; i++) {
        shadow += shadowmapShading (vsv, poissonDisk[i] / 1000, bias);
    }
    shadow /= 4;

    if (shadow > 0.0001)
    {
        for (int j = 1; j <= 3; j++) {
            for (int i = 0; i < 4; i++) {
                shadow += shadowmapShading (vsv, poissonDisk[i] / (10000 / (j)), bias);
            }
        }

        shadow /= 13;
    }

    float depthDiff = abs (texture (uShadowMapFlat, vsv.xy).x - vsv.z);
    float addShadow = depthDiff * 180;
    shadow += addShadow;
    shadow = clamp (shadow, 0, 1);

    shadow = 0.3 + shadow * 0.7;

    gl_FragData[0] = vec4 (diff * shadow, 1);

    vec3 r = reflect (-vert2Eye, normal);
    //gl_FragData[0] = vec4 ((diff + vec3 (1, 1, 1) * pow (max (dot (light2Vert, r), 0.0), 60)) * shadow, 1);
    //gl_FragColor = vec4 (diff + vec3 (1, 1, 1) * pow (max (dot (light2Vert, r), 0.0), 60), 1);
}
