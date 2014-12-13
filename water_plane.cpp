
#include "water_plane.hpp"

#include "gpu_buffer_impl.hpp"
#include "mesh_impl.hpp"
#include "render_resources.hpp"
#include "resource_manager_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

void water_plane::draw (graphics_renderer &renderer) const
{
    //glDisable (GL_CULL_FACE);
    renderer.use (renderer.state().activeCamera()->object2ScreenTransform (transform_d()));
    _mesh->draw (renderer);
    //glEnable (GL_CULL_FACE);
}


water_plane::water_plane (resources& renderRes, render_window &renderWindow, float y) :
      _surfaceHeight (y)
{
    auto waterplaneShaderId = gpu_program::id (elementary_shapes::simple_vertex_layout::alloc(),
                                               "special/water_plane.vert", "special/water_plane.frag");
    auto waterplaneShader = renderRes.gpuProgramsManager().request (waterplaneShaderId, renderRes);
    _material = material::alloc (technique::alloc (waterplaneShader));

    vector<elementary_shapes::simple_vertex> vertices;
    vector<unsigned short> indices;
    elementary_shapes::quadXZ (vertices, indices, 2000, y);
    _mesh = mesh_component<elementary_shapes::simple_vertex, unsigned short>::alloc (
            _material, vertices, indices, "waterplane");
    _mesh->backfaceCulling (false);

    auto normalMap = renderRes.requestFromFile<texture> ("water-normal.jpg");
    normalMap->filtering (texture::linear_MipmapLinear, texture::linear_MipmapLinear);
    _material->set ("uNormalMap", normalMap);

    _reflectionsRT = offscreen_render_target::alloc (renderWindow.size() / 2, 1, true);

    unique_ptr<perspective_projection_d> projection (new perspective_projection_d (
            angle_d::pi / 4, renderWindow.aspectRatio (), interval_d (1, 1000)));

    _reflectionsCamera = camera::alloc (std::move (projection));

    _material->set ("uReflection", _reflectionsRT->colorTexture());
}


void water_plane::useRefractionTextures (texture::ptr refractTexture, texture::ptr depthTexture)
{
    _material->set ("uRefractionDepth", depthTexture);
    _material->set ("uRefraction", refractTexture);
}


void water_plane::drawReflections (graphics_renderer &renderer, scene &reflectibleScene, scene::render_group_id reflectibleGroup)
{
    _reflectionsCamera->asInverseYOf (*(renderer.state().activeCamera()), _surfaceHeight);

    _reflectionsRT->setup (renderer);
    renderer.use (_reflectionsCamera);

    glEnable (GL_CLIP_DISTANCE0);

    reflectibleScene.draw (renderer, reflectibleGroup);

    glDisable (GL_CLIP_DISTANCE0);
}
