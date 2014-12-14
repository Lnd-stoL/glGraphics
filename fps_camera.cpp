
#include "fps_camera.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

fps_camera_controller::fps_camera_controller (render_window &window, render::camera::ptr controlledCamera) :
    _controlledCamera (controlledCamera),
    _window (window),
    _lastMousePos (sf::Mouse::getPosition())
{
    window.frameUpdateEvent().handleWith ([this](double time)  { _handleFrameUpdate (time); });

}


void fps_camera_controller::_handleFrameUpdate (double time)
{
    vector3_d  newTranslation = _updateTranslation (time);
    rotation_d newRotation    = _recalcRotation (time);

    if (_constraint (newTranslation + _controlledCamera->transform().ttranslation(), newRotation))
        _controlledCamera->translateChangeRotation (newTranslation, newRotation);
}


vector3_d fps_camera_controller::_updateTranslation (double time)
{
    vector3_d translation;

    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::D))  translation +=  _controlledCamera->rightVector();
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::A))  translation += -_controlledCamera->rightVector();
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::W))  translation +=  _controlledCamera->forwardVector();
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::S))  translation += -_controlledCamera->forwardVector();

    return translation * _translationSpeed * time;
}


rotation_d fps_camera_controller::_recalcRotation (double time)
{
    //auto eulerAngles = _controlledCamera->transform().trotation().asEulerAngles();
    //_yaw = eulerAngles.yaw;
    //_pitch = eulerAngles.pitch;

    const double keyRotationCoeff = time * 10;
    angle_d keyDeltaAngle = _rotationSpeed * keyRotationCoeff;

    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Left))   _pitch += -keyDeltaAngle;
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Right))  _pitch +=  keyDeltaAngle;
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Up))     _yaw   += -keyDeltaAngle;
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Down))   _yaw   +=  keyDeltaAngle;

    auto curMousePos = sf::Mouse::getPosition();

    double mouseDeltaX = curMousePos.x - _lastMousePos.x;
    double mouseDeltaY = curMousePos.y - _lastMousePos.y;
    _lastMousePos = curMousePos;

    if (sf::Mouse::isButtonPressed (sf::Mouse::Left))
    {
        angle_d deltaYaw   = mouseDeltaY * _rotationSpeed * time;
        angle_d deltaPitch = mouseDeltaX * _rotationSpeed * time;

        const double trasholdAngle = 0.5;  // just to avoid some stragne platform-dependent issues
        if (std::abs (deltaYaw) < trasholdAngle && std::abs (deltaPitch) < trasholdAngle)
        {
            _yaw   += deltaYaw;
            _pitch += deltaPitch;
        }
    }

    if (std::abs (_yaw) > _YawLimit)  _yaw = _YawLimit * (std::abs (_yaw) / _yaw);

    rotation_d newRotation (vector3_d (0, 1, 0), _pitch);
    newRotation.combine (rotation_d (vector3_d (1, 0, 0), _yaw));

    return newRotation;
}


void fps_camera_controller::constraint (std::function<bool(const vector3_d&, const rotation_d&)> criteria)
{
    _constraint = criteria;
}


void fps_camera_controller::tune (config_set::ptr config)
{
    config = config->group ("fps_camera");
    _translationSpeed = config->get<double> ("translation_speed");
    _rotationSpeed    = config->get<double> ("rotation_speed");
}
