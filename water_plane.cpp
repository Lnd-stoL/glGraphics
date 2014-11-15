
#include "water_plane.hpp"

#include "gpu_buffer_impl.hpp"
#include "mesh_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

void water_plane::draw (graphics_renderer &renderer) const
{
    _mesh->draw (renderer);
}


water_plane::water_plane (resources& renderRes, render_window &renderWindow, transform_d const &transform)
        : _transform (transform)
{
    auto waterplaneShaderId = gpu_program::id (elementary_shapes::simple_vertex_layout::alloc(),
                                               "water_plane.vert", "water_plane.frag");
    auto waterplaneShader = renderRes.gpuProgramsManager().request (waterplaneShaderId, renderRes);
    _material = material::alloc (technique::alloc (waterplaneShader));

    vector<elementary_shapes::simple_vertex> vertices;
    vector<unsigned short> indices;
    elementary_shapes::quadXZ (vertices, indices, 2000, 10);
    _mesh = mesh_component<elementary_shapes::simple_vertex, unsigned short>::alloc (
            _material, vertices, indices, "waterplane");

    auto normalMap = renderRes.requestFromFile<texture> ("water-normal.jpg");
    normalMap->filtering (texture::linear_MipmapLinear, texture::linear_MipmapLinear);
    _material->textures()["uNormalMap"] = normalMap;

    _reflectionsFrameBuffer = frame_buffer::alloc (renderWindow.getWidth(), renderWindow.getHeight());
    _reflectionsFrameBuffer->attachDepthTexture();
    _reflectionsTexture = _reflectionsFrameBuffer->attachColorTexture();

    unique_ptr<perspective_projection_d> projection (new perspective_projection_d (
            angle_d::pi / 4, renderWindow.getAspectRatio(), interval_d (1, 1000)));

    _reflectionsCamera = camera::alloc (projection);

    _material->textures()["uReflection"] = _reflectionsTexture;
}


void water_plane::useRefractionTextures (texture::ptr refractTexture, texture::ptr depthTexture)
{
    _material->textures()["uRefractionDepth"] = depthTexture;
    _material->textures()["uRefraction"] = refractTexture;
}


void water_plane::drawReflections (graphics_renderer &renderer, scene &reflectibleScene)
{
    _renderer.state().getCamera()

    renderer.renderTo (_reflectionsFrameBuffer);
    reflectibleScene.draw (renderer);
}
