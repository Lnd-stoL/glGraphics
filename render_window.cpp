
#include "render_window.hpp"

#include <glbinding/Binding.h>

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

render_window::render_window (unsigned width, unsigned height, const string &title) : _width (width), _height (height)
{
    if (!glfwInit())
        return;

    glfwDefaultWindowHints();
    glfwWindowHint (GLFW_SAMPLES, 0);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, false);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _window = glfwCreateWindow (width, height, title.c_str(), nullptr, nullptr);
    if (!_window)
    {
        glfwTerminate();
        return;
    }

    //glfwSetKeyCallback (window, key_callback);
    //glfwSetFramebufferSizeCallback (window, framebuffer_size_callback);
    glfwMakeContextCurrent (_window);

    //_window.setFramerateLimit (60);

    debug::log::println ("initializing OpenGL binding ...");

    glbinding::Binding::initialize();

    /*auto contextSettings = _window.getSettings();
    debug::log::println (mkstr ("created rendering window & context: OpenGL ",
                                contextSettings.majorVersion, ".", contextSettings.minorVersion));
                                */

    //glEnable (GL_DEPTH_TEST);
    //glEnable (GL_CULL_FACE);
    //glDisable (GL_MULTISAMPLE);

    //glActiveTexture (GL_TEXTURE0);
    //glEnable (GL_TEXTURE_2D);
    //glActiveTexture (GL_TEXTURE1);
    //glEnable (GL_TEXTURE_2D);
    //glActiveTexture (GL_TEXTURE2);
    //glEnable (GL_TEXTURE_2D);

}


void render_window::runEventLoop()
{
    while (!glfwWindowShouldClose (_window))
    {
        glfwPollEvents();

        _frameUpdateEvent (*this);
        _frameDrawEvent (*this);

        glfwSwapBuffers (_window);
    }

    /*
    while (_window.isOpen())
    {
        sf::Event event;
        while (_window.pollEvent (event))
        {
            if (event.type == sf::Event::Closed)  _window.close();

            else if (event.type == sf::Event::Resized)
            {
                _handleWindowResize (event.size.width, event.size.height);
            }

            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape) _window.close();
                if (event.key.code == sf::Keyboard::F12)
                {
                    auto tt = time (nullptr);
                    saveScreenshot (mkstr ("screenshots/screenshot-", tt, ".jpg"));
                }
            }
        }

        _frameUpdateEvent (*this);

        glEnable (gl::GL_DEPTH_TEST);
        glEnable (gl::GL_CULL_FACE);

        _frameDrawEvent (*this);

        _window.display();
    }
    */
}


void render_window::_handleWindowResize (unsigned width, unsigned height)
{
    //glFrustum(-1, 1, -1, 1, 1, 100000);
    _sizeChangedEvent (width, height);
    glViewport (0, 0, width, height);
}


double render_window::getAspectRatio() const
{
    return _width / _height;
}


void render_window::saveScreenshot (const string &fileToSave)
{
    //_window.capture().saveToFile (fileToSave);
}


void render_window::clear()
{
    glClearColor (0, 0, 0, 1);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


unsigned render_window::getWidth() const
{
    return _width;
}


unsigned render_window::getHeight() const
{
    return _height;
}
