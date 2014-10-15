
#include "mesh_renderable_object.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

mesh_renderable_object::mesh_renderable_object (static_mesh::ptr mesh, const transform_d &transform) : _mesh (mesh),
                                                                                                       _transform (transform)
{ }


/*virtual*/ void mesh_renderable_object::draw (const camera &viewer) const
{
    object2screen_transform_d screenTransform (_transform, viewer.getTransform(), viewer.getProjection());
    _mesh->draw (screenTransform);
}
