
#include "volumetric_fog.hpp"

#include "gpu_buffer_impl.hpp"
#include "mesh_impl.hpp"
#include "render_resources.hpp"
#include "resource_manager_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

volumetric_fog::volumetric_fog (resources &renderRes, a_mesh_component::ptr mesh, const transform_d &trans) :
    transformable_renderable_object (trans),
    _mesh (mesh)
{
    auto shaderId = gpu_program::id (exs3d_mesh::exs3d_vertex_layout::alloc(), "utils/simple.vert",
                                     "special/volumetric_fog.frag");

    auto shader = renderRes.gpuProgramsManager().request (shaderId, renderRes);
    _material = material::alloc (technique::alloc (shader));

    mesh->changeMaterial (_material);
}


void volumetric_fog::useDepthTexture (texture::ptr depthTexture)
{
    _material->set ("uDepthMap", depthTexture);
}


void volumetric_fog::useColorTexture (texture::ptr colorTexture)
{
    _material->set ("uColorMap", colorTexture);
}


void volumetric_fog::draw (graphics_renderer &renderer) const
{
    transformable_renderable_object::_setupObject2ScreenTransform (renderer);

    renderer.blend (true);
    renderer.testDepth (true);

    _mesh->draw (renderer);
}


volumetric_fog::ptr volumetric_fog::createLayer (resources& renderRes, interval_d heightInterval, vector2_d size)
{
    auto cubeMesh = renderRes.requestFromFile<exs3d_mesh> ("cube.exs3d");
    transform_d trans (vector3_d (0, heightInterval.average(), 0), rotation_d(),
                       vector3_d (size.x(), heightInterval.length(), size.y()));

    auto mesh = cubeMesh->renderableMesh()->component ("Cube");
    //mesh->backfaceCulling (false);
    return volumetric_fog::alloc (renderRes, mesh, trans);
}
