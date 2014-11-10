
#ifndef __water_plane__included__
#define __water_plane__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "mesh.hpp"
#include "renderable.hpp"
#include "resource_manager.hpp"

using namespace render;

using math3D::transform_d;
using math3D::vector3_d;
using math3D::object2screen_transform_d;
using math3D::perspective_projection_d;

//----------------------------------------------------------------------------------------------------------------------
/*
class water_material : public material
{
protected:
    texture::ptr _texture;

public:
    declare_ptr_alloc (water_material)
    water_material (resources &renderRes);

    //virtual void use() const;
    //virtual void setupViewerTransform (const math3D::object2screen_transform_d &transform);
};
*/
//----------------------------------------------------------------------------------------------------------------------

class water_plane : public renderable
{
    mesh_component<elementary_shapes::simple_vertex, unsigned short>::ptr  _mesh;
    transform_d _transform;

public:
    property_get (Mesh, _mesh)
    property_get_ref (Transform, _transform)


public:
    declare_ptr_alloc (water_plane)
    water_plane (const transform_d &transform = transform_d::ident());

    virtual void draw (graphics_renderer &renderer) const;
};

//----------------------------------------------------------------------------------------------------------------------

#endif
