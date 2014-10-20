
#ifndef __camera_included__
#define __camera_included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"

using namespace math3D;
using oo_extensions::event;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class camera
    {
    protected:
        transform_d  _transform         = transform_d::ident();
        transform_d  _inversedTransform = transform_d::ident();
        unique_ptr<projection_d>  _projection;

        vector3_d _up      = vector3_d (0, 1, 0);
        vector3_d _right   = vector3_d (1, 0, 0);
        vector3_d _forward = vector3_d (0, 1, -1);

    public:
        property_get_ref (Transform,         _transform)
        property_get_ref (InversedTransform, _inversedTransform)

        property_get_ref (UpVector,      _up)
        property_get_ref (RightVector,   _right)
        property_get_ref (ForwardVector, _forward)


    protected:
        void _updateBasis();
        void _updateCached();

    public:
        declare_ptr_alloc (camera)

        camera (unique_ptr<projection_d> &&proj) : _projection (std::move (proj))
        { }

        void addTransform (const transform_d &deltaTransform);
        void translateRotate (const vector3_d &trans, const rotation_d &rot);
        void changeProjection (unique_ptr<projection_d> &&proj);
        projection_d* getProjection() const;

        void syncProjectionAspectRatio (event<unsigned, unsigned> &sizeChangeEvent);
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
