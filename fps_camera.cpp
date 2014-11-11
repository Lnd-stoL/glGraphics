
#include "fps_camera.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

fps_camera_controller::fps_camera_controller (render_window &window, render::camera::ptr controlledCamera) :
    _controlledCamera (controlledCamera),
    _window (window),
    _lastMousePos (sf::Mouse::getPosition (window.sfmlWindow()))
{
    window.frameUpdateEvent().handleWith ([this](const render_window&) { _handleFrameUpdate(); });

}


void fps_camera_controller::_handleFrameUpdate()
{
    _controlledCamera->translateChangeRotation (_updateTranslation(), _recalcRotation());
}


vector3_d fps_camera_controller::_updateTranslation()
{
    vector3_d translation;

    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::D))  translation +=  _controlledCamera->getRightVector();
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::A))  translation += -_controlledCamera->getRightVector();
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::W))  translation +=  _controlledCamera->getForwardVector();
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::S))  translation += -_controlledCamera->getForwardVector();

    return translation * _translationSpeed;
}


rotation_d fps_camera_controller::_recalcRotation()
{
    const double keyRotationCoeff = 10;
    angle_d keyDeltaAngle = _rotationSpeed * keyRotationCoeff;

    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Left))   _pitch += -keyDeltaAngle;
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Right))  _pitch +=  keyDeltaAngle;
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Up))     _yaw   += -keyDeltaAngle;
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Down))   _yaw   +=  keyDeltaAngle;

    auto curMousePos = sf::Mouse::getPosition (_window.getSfmlWindow());

    double mouseDeltaX = curMousePos.x - _lastMousePos.x;
    double mouseDeltaY = curMousePos.y - _lastMousePos.y;
    _lastMousePos = curMousePos;

    if (sf::Mouse::isButtonPressed (sf::Mouse::Left))
    {
        angle_d deltaYaw   = mouseDeltaY * _rotationSpeed;
        angle_d deltaPitch = mouseDeltaX * _rotationSpeed;

        const double trasholdAngle = 0.5;  // just to avoid some stragne platform-dependent issues
        if (std::abs (deltaYaw) < trasholdAngle && std::abs (deltaPitch) < trasholdAngle)
        {
            _yaw   += deltaYaw;
            _pitch += deltaPitch;
        }
    }

    if (std::abs (_yaw) > _YawLimit)  _yaw = _YawLimit * (_yaw / _yaw);

    rotation_d newRotation (vector3_d (0, 1, 0), _pitch);
    newRotation.combine (rotation_d (vector3_d (1, 0, 0), _yaw));

    return newRotation;
}
