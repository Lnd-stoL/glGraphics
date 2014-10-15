
#ifndef __mesh_renderable_object__included__
#define __mesh_renderable_object__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "mesh.hpp"
#include "renderable.hpp"

using namespace render;

using math3D::transform_d;
using math3D::vector3_d;
using math3D::object2screen_transform_d;
using math3D::perspective_projection_d;

//----------------------------------------------------------------------------------------------------------------------

class mesh_renderable_object :
        public renderable
{
    static_mesh::ptr  _mesh;
    transform_d       _transform;

public:
    property_get (Mesh, _mesh)
    property_get_ref (Transform, _transform)


public:
    declare_ptr_alloc (mesh_renderable_object)
    mesh_renderable_object (static_mesh::ptr mesh, const transform_d &transform = transform_d::ident());

    virtual void draw (const camera &viewer) const;
};

//----------------------------------------------------------------------------------------------------------------------

#endif
