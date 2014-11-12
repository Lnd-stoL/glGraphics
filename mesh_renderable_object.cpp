
#include "mesh_renderable_object.hpp"
#include "scene.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

mesh_renderable_object::mesh_renderable_object (mesh::ptr mesh, const transform_d &transform) : _mesh (mesh),
                                                                                                _transform (transform)
{ }


/*virtual*/ void mesh_renderable_object::draw (graphics_renderer &renderer) const
{
    auto usedCamera = renderer.state().getCamera();
    renderer.use (object2screen_transform_d (_transform, usedCamera->getTransform(), usedCamera->getProjection()));

    _mesh->draw (renderer);
}
