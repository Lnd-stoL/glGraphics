
#version 120

uniform sampler2D uTexture;

varying vec3 vNormal;
varying vec3 vLight2VertPos;
varying vec2 vTexUV;
varying vec3 vVert2Eye;


void main()
{
    gl_FragColor = vec4 (1, 1, 1, 1);

    vec3 normal     = normalize (vNormal);
    vec3 light2Vert = normalize (vLight2VertPos);
    vec3 vert2Eye   = normalize (vVert2Eye);

    vec3 r = reflect (-vert2Eye, normal);

    vec3 diff = texture2D (uTexture, vTexUV).xyz * max (0.1, dot (normal, light2Vert));
    gl_FragColor *= vec4 (diff + vec3 (1, 1, 1) * pow (max (dot (light2Vert, r), 0.0), 40), 1);
}
