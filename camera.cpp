
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


    void camera::changeProjection (unique_ptr<projection_d> &&proj)
    {
        _projection = unique_ptr<projection_d> (std::move (proj));
        //_screenTransform = _screenTransform.withChangedProjection (projection);
    }


    void camera::syncProjectionAspectRatio (event<unsigned, unsigned> &sizeChangeEvent)
    {
        sizeChangeEvent.handleWith ([this] (unsigned newWidth, unsigned newHeight) {
            _projection->changeAspect (newWidth / newHeight);
        });
    }

    projection_d *camera::getProjection() const
    {
        return _projection->copy();
    }
}
