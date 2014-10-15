
#include "camera.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    void camera::addTransform (const transform_d &deltaTransform)
    {
        _transform.add (deltaTransform);
        _updateCached();
    }


    void camera::_updateBasis()
    {
        _up      = _inversedTransform.getRotation().apply (vector3_d (0, 1, 0));
        _right   = _inversedTransform.getRotation().apply (vector3_d (1, 0, 0));
        _forward = _up.crossProduct (_right);
    }


    void camera::translateRotate (const vector3_d &trans, const rotation_d &rot)
    {
        _transform.translate (trans);
        _transform.rotate (rot);
        _updateCached();
    }


    void camera::_updateCached()
    {
        _inversedTransform = _transform.inversed();
        _updateBasis();
        //_screenTransform = _screenTransform.withChangedCameraTransform (_transform);
    }


    void camera::changeProjection (const perspective_projection_d &projection)
    {
        _projection = projection;
        //_screenTransform = _screenTransform.withChangedProjection (projection);
    }
}
