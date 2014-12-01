
#version 330 core

uniform sampler2D uTexture;
uniform sampler2D uNormalmap;
uniform sampler2D uHeightmap;
uniform sampler2DShadow uShadowMap;
uniform sampler2D uShadowMapFlat;

uniform vec3 uLightColor;
uniform vec3 uLightPos;

in vec3 gNormal;
in vec3 gLight2VertPos;
in vec2 gTexUV;
in vec3 gVert2Eye;
in vec4 gShadowmapVert;
in vec3 gViewSpaceNormal;
in vec3 gViewSpaceCoords;
in vec3 gWorldSpaceCoords;

in vec3 gTangent;
in vec3 gBitangent;
in vec3 gFaceNormal;
in vec3 gVert2EyeTBN;

out vec3  out_Color;
out vec3  out_Normal;


vec2 normal_encode (vec3 n)
{
    vec2 enc = normalize (n.xy) * (sqrt (-n.z * 0.5 + 0.5));
    enc = enc*0.5 + 0.5;
    return enc;
}


mat3 cotangent_frame (vec3 N, vec3 p, vec2 uv)
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );

    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // construct a scale-invariant frame
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}


vec3 perturb_normal (vec3 N, mat3 TBN, vec2 texcoord)
{
    vec3 map = texture (uNormalmap, texcoord).rgb * 2 - 1;
    return normalize (TBN * map);
}


void main()
{
    vec3 smoothNormal     = normalize (gNormal);
    vec3 light2Vert = normalize (uLightPos - gWorldSpaceCoords);
    vec3 vert2Eye   = normalize (gVert2Eye);

    //normal = -normalize (cross (dFdy (gWorldSpaceCoords), dFdx (gWorldSpaceCoords)));
    vec3 normal = normalize (gFaceNormal);
    mat3 TBN = mat3 (normalize (gTangent), normalize (gBitangent), normalize (normal));
    //mat3 TBN = cotangent_frame (smoothNormal, vert2Eye, gTexUV);
    vec3 vert2EyeCtg = normalize (gVert2EyeTBN);
    //vec3 vert2EyeCtg = normalize (transpose (TBN) * vert2Eye);

    vec2 tex = gTexUV - ((texture (uHeightmap, gTexUV).r) * 0.022 + 0.002) * vert2EyeCtg.xy;
    //tex = vTexUV;

    vec3 et = vert2EyeCtg;
    float scale = 0.09;
    int numSteps = 20;
    float   step   = 1.0 / numSteps;                      // distance between checked layers
    vec2    dtex   = et.xy * scale / (numSteps * et.z); // adjustment for one layer
    float   height = 1;                                 // height of the layer
            tex    = gTexUV;                  // our initial guess
    float   h      = texture ( uHeightmap, tex ).r;      // get height

    while ( h < height )                               // check every layer
    {
        height -= step;                                   // update height
        tex    += dtex;                                   // update texture coordinates
        h       = texture ( uHeightmap, tex ).r + 0.001;         // get new height
    }

    // now find point via linear interpolation
    vec2  prev   = tex - dtex;                          // previous point
    float hPrev  = texture (uHeightmap, prev ).r - (height + step);
    float hCur   = h - height;
    float weight = hCur / (hCur - hPrev);

    tex = weight * prev + (1.0 - weight) * tex;         // interpolate to get tex coords

    vec2 texUV = tex;
    vec3 diffColor  = texture2D (uTexture, texUV).xyz;
    normal = perturb_normal (smoothNormal, TBN, texUV);
    float shadow = 1.0;
    float diffLight = max (0, dot (normal, light2Vert));
    vec3 spec = vec3 (1) * pow ( max ( dot (light2Vert, reflect (-vert2Eye, normal)), 0), 40);
    out_Color = ((diffLight*0.7 + 0.3) * shadow) * uLightColor * diffColor + spec;

    //out_Color = diffColor;
    //out_Color = texture (uHeightmap, vTexUV).rgb;
    //out_Color = vec3 (dot (TBN2[0], TBN2[1]), dot (TBN2[0], TBN2[2]), dot (TBN2[2], TBN2[1]));
    //out_Color = abs (vert2EyeCtg - normalize (transpose (TBN) * vert2Eye));

    out_Normal = vec3 (normal_encode (gViewSpaceNormal), 0);
}
