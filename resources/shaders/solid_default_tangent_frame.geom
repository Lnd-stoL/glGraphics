
#version 330 core

//----------------------------------------------------------------------------------------------------------------------

layout (triangles)  in;
layout (triangle_strip, max_vertices = 3)  out;

//----------------------------------------------------------------------------------------------------------------------

in vec3 vNormal[];
in vec3 vLight2Vert[];
in vec2 vTexUV[];
in vec4 vShadowmapVert[];
in vec3 vViewSpaceNormal[];
in vec3 vVert2Eye[];
in vec3 vWorldSpaceCoords[];

//----------------------------------------------------------------------------------------------------------------------

out vec3 gNormal;
out vec3 gLight2Vert;
out vec2 gTexUV;
out vec4 gShadowmapVert;
out vec3 gViewSpaceNormal;
out vec3 gWSC;
out vec3 gVert2Eye;

out vec3 gTangent;
out vec3 gBitangent;
out vec3 gFaceNormal;
out vec3 gVert2EyeTBN;

//----------------------------------------------------------------------------------------------------------------------

mat3 calculate_tangentspace()
{
    vec3 dXYZdU = vWorldSpaceCoords[1] - vWorldSpaceCoords[0];
    vec3 dXYZdV = vWorldSpaceCoords[2] - vWorldSpaceCoords[0];
    vec2 dSTdU  = vTexUV[1] - vTexUV[0];
    vec2 dSTdV  = vTexUV[2] - vTexUV[0];

    vec3 normal = normalize (cross (dXYZdU, dXYZdV));

    float f = 1 / (dSTdU.s * dSTdV.t - dSTdU.t * dSTdV.s);
    vec3 tangent = normalize (-f * (dSTdV.y * dXYZdU - dSTdU.y * dXYZdV));

    float area = determinant (mat2 (dSTdV, dSTdU));
    tangent = area >= 0 ? tangent : -tangent;
    tangent = normalize (tangent - dot (tangent, normal) * normal);

    vec3 bitangent = normalize (cross (tangent, normal));
    return mat3 (tangent, bitangent, normal);
}


void main()
{
    mat3 tbn = calculate_tangentspace();
    mat3 t_tbn = transpose (tbn);

    for (int i = 0; i < gl_in.length(); ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        
        gNormal          = vNormal[i];
        gLight2Vert      = vLight2Vert[i];
        gTexUV           = vTexUV[i];
        gViewSpaceNormal = vViewSpaceNormal[i];
        gVert2Eye        = vVert2Eye[i];
        gShadowmapVert   = vShadowmapVert[i];
        gWSC = vWorldSpaceCoords[i];

        gTangent     = tbn[0];
        gBitangent   = tbn[1];
        gFaceNormal  = tbn[2];
        gVert2EyeTBN = t_tbn * vVert2Eye[i];

        gl_ClipDistance[0] = gl_in[i].gl_ClipDistance[0];
        EmitVertex();
    }

    EndPrimitive();
}
