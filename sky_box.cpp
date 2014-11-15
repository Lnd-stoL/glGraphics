
#include "sky_box.hpp"

#include "gpu_buffer_impl.hpp"
#include "mesh_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

void sky_box::draw (graphics_renderer &renderer) const
{
    renderer.use (renderer.state().getCamera()->object2ScreenTransform (_transform));
    _mesh->draw (renderer);
}


sky_box::sky_box (resources& renderRes)
        : _transform (transform_d::ident()),
{
    auto skyboxShaderId = gpu_program::id (elementary_shapes::simple_vertex_layout::alloc(),
                                               "water_plane.vert", "water_plane.frag");
    auto skyboxShader = renderRes.gpuProgramsManager().request (skyboxShaderId, renderRes);
    _material = material::alloc (technique::alloc (skyboxShader));

    vector<elementary_shapes::simple_vertex> vertices;
    vector<unsigned short> indices;
    elementary_shapes::quadXZ (vertices, indices, 2000, y);
    _mesh = mesh_component<elementary_shapes::simple_vertex, unsigned short>::alloc (
            _material, vertices, indices, "waterplane");


}
