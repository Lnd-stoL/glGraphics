
#ifndef __sky_box__included__
#define __sky_box__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "mesh.hpp"
#include "renderable.hpp"
#include "render_resources.hpp"
#include "scene.hpp"

using namespace render;

//----------------------------------------------------------------------------------------------------------------------

class sky_box : public renderable
{
    mesh::ptr  _mesh;
    transform_d  _transform;
    material::ptr  _material;

public:
    property_get (Mesh, _mesh)
    property_get_ref (Transform, _transform)


public:
    declare_ptr_alloc (sky_box)
    sky_box (resources& renderRes);

    void update (math3D::vector3_f sunPosition);
    virtual void draw (graphics_renderer &renderer) const;
};

//----------------------------------------------------------------------------------------------------------------------

#endif
