
#version 130

uniform sampler2D  uNormalMap;
uniform sampler2D  uRefractionDepth;
uniform sampler2D  uRefraction;
uniform sampler2D  uReflection;

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
    vec3 waterColor = vec3 (0.1, 0.6, 0.8);
    vec3 waterSpecular = vec3 (0.9, 0.9, 1);
    vec3 vert2Eye   = normalize (vVert2Eye);

    vec3 light2Vert = normalize (vLight2VertPos);
    vec3 normalPixel = texture (uNormalMap, vTexUV).xzy + texture (uNormalMap, -vTexUV / 5).xzy + 0.5 * texture (uNormalMap, vTexUV * 3).xzy;
    normalPixel /= 2.5;
    vec3 normal = normalize (normalPixel * 2 - vec3 (1) + vec3 (0, 0.7, 0));
    float diffuseLight = clamp (dot (normal, light2Vert), 0.1, 1) * 0.9 + 0.1;

    vec3 r = reflect (-light2Vert, normal);
    //diffuseLight = 1;
    vec3 outColor = diffuseLight * waterColor + waterSpecular * pow (max (dot (vert2Eye, r), 0.0), 200);


    float density = 0.6;
    vec3 proj_tc  = 0.5 * proj_coords.xyz / proj_coords.w + 0.5;
    float fresnel = 1.0 - dot (vert2Eye, normal);
    float fOwnDepth = calculate_linear_depth (proj_tc.z);
    float fSampledDepth = texture (uRefractionDepth, proj_tc.xy).x;
    fSampledDepth       = calculate_linear_depth (fSampledDepth);
    float fLinearDepth  = fSampledDepth - fOwnDepth;

    float fExpDepth = 1.0 - exp( -density * fLinearDepth);
    float fExpDepthHIGH = 1.0 - exp( -0.95 * fLinearDepth );

    // величина искажения – чем глубже, тем искажения больше
    float fDistortScale = 0.2 * fExpDepth;
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
        vDistort = vec2(0.0);
        fDistortedExpDepth = fExpDepth;
    }
    // теперь читаем из текстуры преломлений цвет
    vec3 refraction = texture (uRefraction, proj_tc.xy + vDistort).xyz;
    // и закрашиваем его цветом воды, в зависимости от глубины
    refraction = mix (refraction, outColor, fDistortedExpDepth);

    vDistort = normal.xz * 0.025;
    vec3 reflection = texture (uReflection, proj_tc.st + vDistort).xyz;
    float fMix = fresnel * fExpDepthHIGH;
    outColor = mix (refraction, reflection, fMix);

    gl_FragData[0] = vec4 (outColor, 1);
}
