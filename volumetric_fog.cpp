
#include "volumetric_fog.hpp"

#include "gpu_buffer_impl.hpp"
#include "mesh_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

volumetric_fog::volumetric_fog (resources &renderRes, a_mesh_component::ptr mesh, const transform_d &trans) :
    transformable_renderable_object (trans),
    _mesh (mesh)
{
    auto shaderId = gpu_program::id (exs3d_mesh::exs3d_vertex_layout::alloc(), "simple.vert", "volumetric_fog.frag");
    auto shader = renderRes.gpuProgramsManager().request (shaderId, renderRes);
    _material = material::alloc (technique::alloc (shader));
    //mesh->changeMaterial (_material);
}


void volumetric_fog::useDepthTexture (texture::ptr depthTexture)
{
    _material->textures()["uDepthMap"] = depthTexture;
}


void volumetric_fog::useColorTexture (texture::ptr colorTexture)
{
    _material->textures()["uColorMap"] = colorTexture;
}


void volumetric_fog::draw (graphics_renderer &renderer) const
{
    transformable_renderable_object::_setupObject2ScreenTransform (renderer);
    //renderer.forceMaterial (_material);
    _mesh->draw (renderer);
    //renderer.stopForcingMaterial();
}


volumetric_fog::ptr volumetric_fog::createLayer (resources& renderRes, interval_d heightInterval, vector2_d size)
{
    auto cubeMesh = renderRes.requestFromFile<exs3d_mesh> ("cube-textured.exs3d");
    transform_d trans (vector3_d (0, heightInterval.average(), 0), rotation_d(),
                       vector3_d (size.getX(), heightInterval.length(), size.getY()));
    return volumetric_fog::alloc (renderRes, cubeMesh->getRenderableMesh()->getComponent ("Cube"), trans);
}
