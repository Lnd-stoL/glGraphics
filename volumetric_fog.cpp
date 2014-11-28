
#include "volumetric_fog.hpp"

#include "gpu_buffer_impl.hpp"
#include "mesh_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

volumetric_fog::volumetric_fog (resources &renderRes, a_mesh_component::ptr mesh, const transform_d &trans) :
    transformable_renderable_object (trans),
    _mesh (mesh)
{
}


void volumetric_fog::useDepthTexture (texture::ptr depthTexture)
{
    _material->textures()["uDepth"] = depthTexture;
}


void volumetric_fog::draw (graphics_renderer &renderer) const
{
    transformable_renderable_object::_setupObject2ScreenTransform (renderer);
    _mesh->draw (renderer);
}
