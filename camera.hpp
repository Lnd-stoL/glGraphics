
#ifndef __camera_included__
#define __camera_included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"

#include <GL/glew.h>

using namespace math3D;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class camera
    {
        transform_d _transform         = math3D::transform_d::ident();
        transform_d _inversedTransform = math3D::transform_d::ident();

        perspective_projection_d _projection;

        vector3_d _up      = vector3_d (0, 1, 0);
        vector3_d _right   = vector3_d (1, 0, 0);
        vector3_d _forward = vector3_d (0, 1, -1);

    public:
        property_get_ref (Transform,         _transform)
        property_get_ref (InversedTransform, _inversedTransform)
        property_get_ref (Projection,        _projection)

        property_get_ref (UpVector,      _up)
        property_get_ref (RightVector,   _right)
        property_get_ref (ForwardVector, _forward)


    protected:
        void _updateBasis();
        void _updateCached();

    public:
        declare_ptr_alloc (camera)

        camera (const perspective_projection_d& projection) : _projection (projection)/*,
                                                              _screenTransform (transform_d::ident(), _transform, _projection)*/
        { }

        void addTransform (const transform_d &deltaTransform);
        void changeProjection (const perspective_projection_d& projection);
        void translateRotate (const vector3_d &trans, const rotation_d &rot);
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
