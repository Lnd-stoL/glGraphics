
#ifndef __volumetric_fog__included__
#define __volumetric_fog__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "mesh.hpp"
#include "renderable.hpp"
#include "render_resources.hpp"
#include "transformable_object.hpp"

using namespace render;
using namespace math3d;

//----------------------------------------------------------------------------------------------------------------------

class volumetric_fog : public transformable_renderable_object
{
    a_mesh_component::ptr  _mesh;
    material::ptr  _material;


public:
    declare_ptr_alloc (volumetric_fog)
    volumetric_fog (resources& renderRes, a_mesh_component::ptr mesh, const transform_d &trans);

    static volumetric_fog::ptr createLayer (resources& renderRes, interval_d heightInterval, vector2_d size);

    void useDepthTexture (texture::ptr depthTexture);
    void useColorTexture (texture::ptr colorTexture);
    virtual void draw (graphics_renderer &renderer) const;
};

//----------------------------------------------------------------------------------------------------------------------

#endif
