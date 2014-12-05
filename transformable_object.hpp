
#ifndef __transformable_object__included__
#define __transformable_object__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "renderable.hpp"
#include "scene.hpp"

using namespace render;

using math3d::transform_d;
using math3d::vector3_d;

//----------------------------------------------------------------------------------------------------------------------

namespace render {  class graphics_renderer; }

//----------------------------------------------------------------------------------------------------------------------

class transformable_renderable_object : public renderable
{
protected:
    transform_d  _transform;

public:
    property_rw_ref (transform, _transform)


protected:
    void _setupObject2ScreenTransform (graphics_renderer &renderer) const;

public:
    declare_ptr (transformable_renderable_object)
    transformable_renderable_object (const transform_d &initialTransform = transform_d());
};

//----------------------------------------------------------------------------------------------------------------------

#endif
