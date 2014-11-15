
#version 130

uniform sampler2D  uNormalMap;
uniform sampler2D  uRefractionDepth;
uniform sampler2D  uRefraction;
uniform sampler2D  uReflection;

uniform float uFrameCount;

varying vec2 vTexUV;
varying vec3 vLight2VertPos;
varying vec3 vVert2Eye;
varying vec4 proj_coords;


float calculate_linear_depth (float value)
{
    float Zfar = 1000;
    float Znear = 1;

    return Zfar * Znear / ( Zfar - value * (Zfar - Znear) );
}


void main()
{
    vec3 waterColor = vec3 (0.3, 0.6, 0.6);
    vec3 waterSpecular = vec3 (0.9, 0.9, 1);
    vec3 vert2Eye   = normalize (vVert2Eye);

    vec3 light2Vert = normalize (vLight2VertPos);
    vec2 texUV = vTexUV + vec2 (uFrameCount / 2);
    vec3 normalPixel = texture (uNormalMap, vTexUV).xzy +
            texture (uNormalMap, -texUV / 5).xzy +
            0.5 * texture (uNormalMap, vTexUV / 20).xzy +
            0.5 * texture (uNormalMap, texUV * 4).xzy;
    normalPixel /= 3;
    vec3 normal = normalize (normalPixel * 2 - vec3 (1) + vec3 (0, 0.2, 0));
    float diffuseLight = clamp (dot (normal, light2Vert), 0, 1) * 0.7 + 0.3;

    vec3 r = reflect (-light2Vert, normal);
    //diffuseLight = 1;
    vec3 outColor = diffuseLight * waterColor;
    vec3 specular = waterSpecular * pow (max (dot (vert2Eye, r), 0.0), 400);


    float density = 0.8;
    vec3 proj_tc  = 0.5 * proj_coords.xyz / proj_coords.w + 0.5;
    float fresnel = 1.0 - dot (vert2Eye, normal);
    float fOwnDepth = calculate_linear_depth (proj_tc.z);
    float fSampledDepth = texture (uRefractionDepth, proj_tc.xy).x;
    fSampledDepth       = calculate_linear_depth (fSampledDepth);
    float fLinearDepth  = fSampledDepth - fOwnDepth;

    float fExpDepth = 1.0 - exp( -density * fLinearDepth);
    float fExpDepthHIGH = 1.0 - exp( -0.96 * fLinearDepth );

    // величина искажения – чем глубже, тем искажения больше
    float fDistortScale = 0.3 * fExpDepth;
    vec2 vDistort = normal.zx * fDistortScale; // смещение текстурных координат
    // читаем глубину в искаженных координатах
    float fDistortedDepth = texture (uRefractionDepth, proj_tc.xy + vDistort).x;
    // преобразуем её в линейную
    fDistortedDepth = calculate_linear_depth (fDistortedDepth);
    float fDistortedExpDepth =
            1.0 - exp( -density * (fDistortedDepth - fOwnDepth) );
    // вычисляем экспоненциальную глубину в искаженных координатах
    // теперь сравниваем расстояния – если расстояние до воды больше,
    // чем до прочитанной то пренебрегаем искажением
    if (fOwnDepth > fDistortedDepth)
    {
        vDistort = vec2 (0.0);
        fDistortedExpDepth = fExpDepth;
    }
    // теперь читаем из текстуры преломлений цвет
    vec3 refraction = texture (uRefraction, proj_tc.xy + vDistort).xyz;
    // и закрашиваем его цветом воды, в зависимости от глубины
    refraction = mix (refraction, waterColor * diffuseLight, fDistortedExpDepth);

    vDistort = normal.xz * 0.055;
    vec3 reflection = texture (uReflection, proj_tc.st + vDistort).xyz;
    float fMix = fresnel * fExpDepthHIGH * 0.9;
    outColor = mix (refraction, reflection, fMix);
    outColor += specular;

    float fog = exp (-0.01 * gl_FragCoord.z);
    fog = 0;
    outColor = (outColor + vec3 (1, 1, 1) * fog) / (1 + fog);

    gl_FragData[0] = vec4 (outColor, 1);
    //gl_FragData[0] = vec4 (1, 0, 0, 1);
}
