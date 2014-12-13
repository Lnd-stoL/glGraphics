
#version 330 core

uniform sampler2D uTexture;
uniform sampler2DShadow uShadowMap;

uniform vec3 uLightColor;
uniform vec3 uLightPos;

in vec3 vNormal;
in vec3 vLight2VertPos;
in vec2 vTexUV;
in vec3 vVert2Eye;
in vec4 vShadowmapVert;
in vec3 vViewSpaceNormal;
in vec3 vViewSpaceCoords;
in vec3 vWorldSpaceCoords;

out vec3  out_Color;
out vec3  out_Normal;


float shadowmapShading (vec3 vsv, vec2 disp, float bias)
{
    return texture (uShadowMap, vec3 (vsv.xy + disp, vsv.z - bias));
}


const vec2 poisson16[] = vec2[](    // These are the Poisson Disk Samples
        vec2( -0.94201624,  -0.39906216 ),
        vec2(  0.94558609,  -0.76890725 ),
        vec2( -0.094184101, -0.92938870 ),
        vec2(  0.34495938,   0.29387760 ),
        vec2( -0.91588581,   0.45771432 ),
        vec2( -0.81544232,  -0.87912464 ),
        vec2( -0.38277543,   0.27676845 ),
        vec2(  0.97484398,   0.75648379 ),
        vec2(  0.44323325,  -0.97511554 ),
        vec2(  0.53742981,  -0.47373420 ),
        vec2( -0.26496911,  -0.41893023 ),
        vec2(  0.79197514,   0.19090188 ),
        vec2( -0.24188840,   0.99706507 ),
        vec2( -0.81409955,   0.91437590 ),
        vec2(  0.19984126,   0.78641367 ),
        vec2(  0.14383161,  -0.14100790 )
);


float random (vec4 seed4)
{
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract (sin (dot_product) * 43758.5453);
}


vec2 normal_encode (vec3 n)
{
    vec2 enc = normalize (n.xy) * (sqrt (-n.z * 0.5 + 0.5));
    enc = enc*0.5 + 0.5;
    return enc;
}


void main()
{
    vec3 normal     = normalize (vNormal);
    vec3 light2Vert = normalize (uLightPos - vWorldSpaceCoords);
    vec3 vert2Eye   = normalize (vVert2Eye);
    vec3 diffColor = texture2D (uTexture, vTexUV).xyz;

    float bias = 0.002 * tan (acos (dot (normal, light2Vert)));
    //bias = clamp (bias, 0.000001, 0.0001);

    vec3 vsv = vShadowmapVert.xyz / vShadowmapVert.w;

    //float depthDiff = abs (texture (uShadowMapFlat, vsv.xy).x - vsv.z);
    //float blurBase = 5000 - (depthDiff * 800) * 3000;
    //blurBase = 3000 + smoothstep (0, 100, -vShadowmapVert.z) * 5000;
    float blurBase = 8000;

    float shadow = 0;
    for (int i = 0; i < 4; i++) {
        shadow += shadowmapShading (vsv, poisson16[i] / blurBase, bias);
    }
    shadow /= 4;

    if (shadow < 0.999)
    {
        for (int j = 1; j <= 2; j++)
            for (int i = 0; i < 4; i++)
            {
                int index = int (16.0 * random (vec4 (gl_FragCoord.xyy, i))) % 16;
                shadow += shadowmapShading (vsv, poisson16[index] / (blurBase / (j)), bias);
            }

        shadow /= 9;
    }

    //shadow = 1 - pow ((1-shadow), 0.5);

    //float depthDiff = abs (texture (uShadowMapFlat, vsv.xy).x - vsv.z);
    //float addShadow = depthDiff * 180;
    //shadow += addShadow;
    shadow = clamp (shadow, 0, 1) * min (dot (normal, normalize (uLightPos / 10 - vWorldSpaceCoords)), 1);
    //shadow = 1;

    float shadowLightenFactor = 0.2;
    shadow = shadowLightenFactor + shadow * (1 - shadowLightenFactor);
    //shadow = 1;

    float fogFactor = 0;
    /*float fogY0 = 0;
    float fogY1 = 2.5;

    float t1 = (fogY1 - vWorldSpaceCoords.y) / vert2Eye.y;
    vec3 intersect1 = vWorldSpaceCoords + t1 * vert2Eye;
    float t0 = (fogY0 - vWorldSpaceCoords.y) / vert2Eye.y;
    vec3 intersect0 = vWorldSpaceCoords + t0 * vert2Eye;

    vec3 fogIn  = intersect0;
    vec3 fogOut = intersect1;

    if (vWorldSpaceCoords.y < fogY1 && vWorldSpaceCoords.y > fogY0)
    {
        if (t1 > 0)  fogIn  = vWorldSpaceCoords;
        else         fogOut = vWorldSpaceCoords;


        float fogLen = length (fogIn - fogOut);
        fogFactor = fogLen / 10;
    }
*/

    //float c = CalculateCaustic (vWorldSpaceCoords, uLightPos);
    float diffLight = max (0, dot (normal, light2Vert));
    out_Color = ((diffLight*0.7 + 0.3) * shadow) * uLightColor * diffColor;
    out_Color = mix (out_Color, (vec3 (1, 1, 1) + 2 * uLightColor) / 3, fogFactor);
    out_Normal = vec3 (normal_encode (vViewSpaceNormal), 0);
}
