
#ifndef __fps_camera_included__
#define __fps_camera_included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "render_window.hpp"
#include "camera.hpp"

#include <SFML/Graphics.hpp>
#include <functional>

//----------------------------------------------------------------------------------------------------------------------

class fps_camera_controller
{
    render::camera::ptr  _controlledCamera;
    const render_window  &_window;

    double _rotationSpeed    = 1.0 / 300.0;
    double _translationSpeed = 1.0 / 3.0;

    sf::Vector2i _lastMousePos;

    angle_d _yaw   = 0;
    angle_d _pitch = 0;

    angle_d _YawLimit = angle_d::pi / 2 - 0.5;


public:
    property_rw (rotationSpeed,    _rotationSpeed)
    property_rw (translationSpeed, _translationSpeed)


protected:
    void _handleFrameUpdate();
    vector3_d _updateTranslation();
    rotation_d _recalcRotation();

public:
    declare_ptr_alloc (fps_camera_controller)
    fps_camera_controller (render_window &window, render::camera::ptr controlledCamera);
};

//----------------------------------------------------------------------------------------------------------------------

#endif


