
#version 120

uniform sampler2D uTexture;
uniform sampler2D uShadowMap;

varying vec3 vNormal;
varying vec3 vLight2VertPos;
varying vec2 vTexUV;
varying vec3 vVert2Eye;
varying vec4 vShadowmapVert;


void main()
{
    //gl_FragColor = vec4 (1, 1, 1, 1);

    vec3 normal     = normalize (vNormal);
    vec3 light2Vert = normalize (vLight2VertPos);
    vec3 vert2Eye   = normalize (vVert2Eye);

    vec3 r = reflect (-vert2Eye, normal);

    vec3 diff = texture2D (uTexture, vTexUV).xyz * max (0.1, dot (normal, light2Vert));
    vec3 vsv = vShadowmapVert.xyz / vShadowmapVert.w;
    float shadow = 1;
    float deltaPixel = 1 / 1000;
    float shadowMapVal = texture2D (uShadowMap, (vsv.xy / 2 + vec2 (0.5, 0.5))).x +
            texture2D (uShadowMap, (vsv.xy / 2 + vec2 (0.5, 0.5) + vec2 (deltaPixel, deltaPixel))).x +
            texture2D (uShadowMap, (vsv.xy / 2 + vec2 (0.5, 0.5) + vec2 (-deltaPixel, -deltaPixel))).x +
            texture2D (uShadowMap, (vsv.xy / 2 + vec2 (0.5, 0.5) + vec2 (-deltaPixel, deltaPixel))).x +
            texture2D (uShadowMap, (vsv.xy / 2 + vec2 (0.5, 0.5) + vec2 (deltaPixel, -deltaPixel))).x;
    shadowMapVal /= 5.0;

    if (shadowMapVal + 0.000001 < (vsv.z * 0.5 + 0.5)) shadow = 0.52;
    gl_FragData[0] = vec4 ((diff + vec3 (1, 1, 1) * pow (max (dot (light2Vert, r), 0.0), 60)) * shadow, 1);
    gl_FragData[0] = vec4 (diff * shadow, 1);
    //gl_FragColor = vec4 (diff + vec3 (1, 1, 1) * pow (max (dot (light2Vert, r), 0.0), 60), 1);
    //gl_FragData[0]
}
