
#ifndef __sky_box__included__
#define __sky_box__included__

//----------------------------------------------------------------------------------------------------------------------

#include "mesh.hpp"
#include "render_resources.hpp"
#include "scene.hpp"

using namespace render;

//----------------------------------------------------------------------------------------------------------------------

class sky_box : public renderable
{
    mesh::ptr  _mesh;
    material::ptr  _material;


public:
    declare_ptr_alloc (sky_box)
    sky_box (resources& renderRes);

    void update (math3d::vector3_f sunPosition);
    virtual void draw (graphics_renderer &renderer) const;
};

//----------------------------------------------------------------------------------------------------------------------

#endif
