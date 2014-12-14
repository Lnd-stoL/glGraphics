
#ifndef __fps_camera_included__
#define __fps_camera_included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "render_window.hpp"
#include "camera.hpp"
#include "config_file.hpp"

#include <SFML/Graphics.hpp>
#include <functional>

using math3d::vector3_d;
using math3d::rotation_d;

//----------------------------------------------------------------------------------------------------------------------

class fps_camera_controller
{
    render::camera::ptr  _controlledCamera;
    const render_window  &_window;
    std::function<bool(const vector3_d&, const rotation_d&)> _constraint;

    double _rotationSpeed    = 1.0 / 30.0;
    double _translationSpeed = 5.0;

    sf::Vector2i _lastMousePos;

    angle_d _yaw   = 0;
    angle_d _pitch = 0;

    angle_d _YawLimit = angle_d::pi / 2 - 0.3;


public:
    property_rw (rotationSpeed,    _rotationSpeed)
    property_rw (translationSpeed, _translationSpeed)


protected:
    void _handleFrameUpdate (double time);
    vector3_d _updateTranslation (double time);
    rotation_d _recalcRotation (double time);


public:
    declare_ptr_alloc (fps_camera_controller)
    fps_camera_controller (render_window &window, render::camera::ptr controlledCamera);
    void tune (config_set::ptr config);

    void constraint (std::function<bool(const vector3_d&, const rotation_d&)> criteria);
};

//----------------------------------------------------------------------------------------------------------------------

#endif


