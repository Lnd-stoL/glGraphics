
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
        _up      = _inversedTransform.trotation().apply (vector3_d (0, 1, 0));
        _right   = _inversedTransform.trotation().apply (vector3_d (1, 0, 0));
        _forward = _up.crossProduct (_right);
    }


    void camera::translateRotate (const vector3_d &trans, const rotation_d &rot)
    {
        _transform.translate (trans);
        _transform.rotate (rot);

        _updateCached();
    }


    void camera::translateChangeRotation (const vector3_d &trans, const rotation_d &rot)
    {
        _transform.translate (trans);
        _transform.changeRotation (rot);

        _updateCached();
    }


    void camera::_updateCached()
    {
        _inversedTransform = _transform.inversed();
        _updateBasis();
    }


    void camera::changeProjection (unique_ptr<projection_d> &&proj)
    {
        _projection = unique_ptr<projection_d> (std::move (proj));
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


    void camera::changeRotation (const rotation_d &rot)
    {
        _transform.changeRotation (rot);
        _updateCached();
    }


    void camera::asInverseYOf (const camera &cam, float y)
    {
        auto trans = cam._transform.ttranslation();
        trans.y (-2*y + trans.y());

        _transform = transform_d (trans, cam._transform.trotation(), vector3_d (1, 1 - 2 * y, 1));

        _updateCached();
    }


    object2screen_transform_d camera::object2ScreenTransform (const transform_d &worldTransform) const
    {
        return object2screen_transform_d (worldTransform, _inversedTransform, getProjection());
    }


    void camera::changeTransform (const vector3_d &trans, const rotation_d &rot)
    {
        _transform.changeTranslation (trans);
        _transform.changeRotation (rot);

        _updateCached();
    }


    void camera::changePosition (const vector3_d &pos)
    {
        _transform.changeTranslation (pos);
        _updateCached();
    }
}
