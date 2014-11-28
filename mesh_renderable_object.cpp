
#include "mesh_renderable_object.hpp"
#include "scene.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

mesh_renderable_object::mesh_renderable_object (mesh::ptr mesh, const transform_d &transform) :
    transformable_renderable_object (transform),
    _mesh (mesh)
{
}


/*virtual*/ void mesh_renderable_object::draw (graphics_renderer &renderer) const
{
    transformable_renderable_object::_setupObject2ScreenTransform (renderer);
    _mesh->draw (renderer);
}
