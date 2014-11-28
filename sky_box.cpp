
#include "sky_box.hpp"

#include "gpu_buffer_impl.hpp"
#include "mesh_impl.hpp"
#include "resource_manager_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

void sky_box::draw (graphics_renderer &renderer) const
{
    glFrontFace (GL_CW);
    renderer.use (renderer.state().getCamera()->object2ScreenTransform (transform_d()));
    _mesh->draw (renderer);
    glFrontFace (GL_CCW);
}


sky_box::sky_box (resources& renderRes)
{
    auto skyboxShaderId = gpu_program::id (exs3d_mesh::exs3d_vertex_layout::alloc(), "sky_box.vert", "sky_box.frag");
    auto skyboxShader = renderRes.gpuProgramsManager().request (skyboxShaderId, renderRes);
    _material = material::alloc (technique::alloc (skyboxShader));

    _mesh = renderRes.requestFromFile<exs3d_mesh> ("sphere.exs3d")->getRenderableMesh();
    _mesh->getComponents()[0]->changeMaterial (_material);

    glEnable (GL_TEXTURE_CUBE_MAP_SEAMLESS);

    string baseFilder = "skybox/day-2/";
    std::map<string, string> cubeMapFaces;
    /*cubeMapFaces["+X"] = renderRes.texturesManager().locateFile (mkstr (baseFilder, "right.jpg"));
    cubeMapFaces["-X"] = renderRes.texturesManager().locateFile (mkstr (baseFilder, "left.jpg"));
    cubeMapFaces["+Y"] = renderRes.texturesManager().locateFile (mkstr (baseFilder, "top.jpg"));
    cubeMapFaces["-Y"] = renderRes.texturesManager().locateFile (mkstr (baseFilder, "bottom.jpg"));
    cubeMapFaces["+Z"] = renderRes.texturesManager().locateFile (mkstr (baseFilder, "front.jpg"));
    cubeMapFaces["-Z"] = renderRes.texturesManager().locateFile (mkstr (baseFilder, "back.jpg"));
    */

    string fileName = "clouds/test.png";
    cubeMapFaces["+X"] = renderRes.texturesManager().locateFile (fileName);
    cubeMapFaces["-X"] = renderRes.texturesManager().locateFile (fileName);
    cubeMapFaces["+Y"] = renderRes.texturesManager().locateFile (fileName);
    cubeMapFaces["-Y"] = renderRes.texturesManager().locateFile (fileName);
    cubeMapFaces["+Z"] = renderRes.texturesManager().locateFile (fileName);
    cubeMapFaces["-Z"] = renderRes.texturesManager().locateFile (fileName);

    //auto cubeMap = texture::alloc (cubeMapFaces);
    //_material->textures()["uSkyBox_CubeMap"] = cubeMap;
    _material->textures()["uClouds"] = renderRes.requestFromFile<texture> (fileName);
}


void sky_box::update (math3d::vector3_f sunPosition)
{
    _material->vec3Params()["lightdir"] = sunPosition;
}
