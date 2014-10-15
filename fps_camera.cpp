
#include "fps_camera.hpp"
#include "debug.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

fps_camera_controller::fps_camera_controller (render_window &window, render::camera::ptr controlledCamera) :
    _controlledCamera (controlledCamera),
    _window (window),
    _lastMousePos (sf::Mouse::getPosition (window.getSfmlWindow()))
{
    window.frameUpdateEvent().handleWith ([this](const render_window&) { _handleFrameUpdate(); });
}


void fps_camera_controller::_handleFrameUpdate()
{
    _controlledCamera->translateRotate (_updateTranslation(), _updateRotation());
}


vector3_d fps_camera_controller::_updateTranslation()
{
    vector3_d translation;

    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::D))  translation += _controlledCamera->getRightVector();
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::A))  translation += -_controlledCamera->getRightVector();
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::W))  translation += _controlledCamera->getForwardVector();
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::S))  translation += -_controlledCamera->getForwardVector();

    translation *= _translationSpeed;
    return translation;
}


rotation_d fps_camera_controller::_updateRotation()
{
    rotation_d deltaRot;
    vector3_d xvec (1, 0, 0);
    vector3_d yvec (0, 1, 0);

    double keyRotationCoeff = 10;
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Left))  deltaRot.combine (rotation_d (yvec, -_rotationSpeed * keyRotationCoeff));
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Right)) deltaRot.combine (rotation_d (yvec,  _rotationSpeed * keyRotationCoeff));
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Up))    deltaRot.combine (rotation_d (xvec, -_rotationSpeed * keyRotationCoeff));
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Down))  deltaRot.combine (rotation_d (xvec,  _rotationSpeed * keyRotationCoeff));

    auto curMousePos = sf::Mouse::getPosition (_window.getSfmlWindow());
    if (sf::Mouse::isButtonPressed (sf::Mouse::Left))
    {
        double mouseDeltaX = curMousePos.x - _lastMousePos.x;
        double mouseDeltaY = curMousePos.y - _lastMousePos.y;
        _lastMousePos = curMousePos;

        if (std::abs (mouseDeltaX) < 100 && std::abs (mouseDeltaY) < 100)
        {
            deltaRot.combine (rotation_d (yvec, _rotationSpeed * mouseDeltaX));

            double deltaYaw = _rotationSpeed * mouseDeltaY;
            _yaw += deltaYaw;
            if (abs (_yaw) < angle_d::pi / 2 - 0.001) deltaRot.combine (rotation_d (xvec, deltaYaw));
        }
    }

    return deltaRot;
}
