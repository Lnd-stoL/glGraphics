
#ifndef __dynamic_sky__included__
#define __dynamic_sky__included__

//----------------------------------------------------------------------------------------------------------------------

#include "mesh.hpp"
#include "render_resources.hpp"
#include "scene.hpp"

using namespace render;

//----------------------------------------------------------------------------------------------------------------------

class dynamic_sky : public renderable
{
    mesh::ptr  _mesh;
    material::ptr  _material;


public:
    declare_ptr_alloc (dynamic_sky)
    dynamic_sky (resources& renderRes);

    void update (math3d::vector3_f sunPosition);
    virtual void draw (graphics_renderer &renderer) const;
};

//----------------------------------------------------------------------------------------------------------------------

#endif
