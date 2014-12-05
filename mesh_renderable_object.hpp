
#ifndef __mesh_renderable_object__included__
#define __mesh_renderable_object__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "mesh.hpp"
#include "transformable_object.hpp"

using namespace render;

//----------------------------------------------------------------------------------------------------------------------

class mesh_renderable_object : public transformable_renderable_object
{
    mesh::ptr   _mesh;

public:
    property_get (underlyingMesh, _mesh)


public:
    declare_ptr_alloc (mesh_renderable_object)
    mesh_renderable_object (mesh::ptr mesh, const transform_d &transform = transform_d::ident());

    virtual void draw (graphics_renderer &renderer) const;
};

//----------------------------------------------------------------------------------------------------------------------

#endif
