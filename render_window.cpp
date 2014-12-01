
#include "render_window.hpp"

#include <glbinding/Binding.h>
#include <glbinding/Meta.h>
#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>

#include <SOIL/SOIL.h>

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

weak_ptr<render_window>  render_window::_singleton;

//----------------------------------------------------------------------------------------------------------------------

render_window::render_window (unsigned width, unsigned height, const string &title) : _width (width), _height (height)
{
    _initWindow (title);

    debug::log::println ("initializing OpenGL lazy binding ...");
    glbinding::Binding::initialize (false);

    debug::log::println_mrk (mkstr ("succesfully initialized ", glbinding::ContextInfo::version(), " context"));
    debug::log::println (mkstr ("rendering device: ", glbinding::ContextInfo::vendor(),
                                " ", glbinding::ContextInfo::renderer()));

    if (glbinding::ContextInfo::version() < glbinding::Version (3, 3))
    {
        debug::log::println_err ("warning: your supported OpenGL version is older than 3.3 core; "
                                 "some feaures or event the whole demo may not work properly");
    }

    glfwSetKeyCallback (_window, _keyboardCallback);
    glfwSetFramebufferSizeCallback (_window, _windowSizeCallback);

    GLuint rootVAO = 0;
    glGenVertexArrays (1, &rootVAO);
    glBindVertexArray (rootVAO);
}


render_window::ptr render_window::create (unsigned width, unsigned height, const string &title)
{
    if (_singleton.lock())
    {
        debug::log::println_err ("multiply render windows are not supported");
        return _singleton.lock();
    }

    auto newRenderWindow = shared_ptr<render_window> (new render_window (width, height, title));
    _singleton = newRenderWindow;
    return newRenderWindow;
}


void render_window::_windowSizeCallback (GLFWwindow* window, int width, int height)
{
    if (_singleton.lock())  _singleton.lock()->_handleWindowResize ((unsigned )width, (unsigned) height);
}


void render_window::_keyboardCallback (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (_singleton.lock())  _singleton.lock()->_handleKeyEvent (key, scancode, action, mods);
}


void render_window::runLoop()
{
    while (!glfwWindowShouldClose (_window))
    {
        glfwPollEvents();
        double startTime = glfwGetTime();

        _frameUpdateEvent (*this);
        _frameDrawEvent (*this);

        glfwSwapBuffers (_window);

        _frameTime = (glfwGetTime() - startTime) * 1000.0;
        _frameTimeSamples++;
        _avgFrameTime += _frameTime;

        if (_frameTimeSamples >= _avgMaxSamplesCount)
        {
            _frameTimeSamples = 0;
            _avgFrameTime = 0;
        }
    }

    debug::log::println ("destroying the rendering window");
    glfwTerminate();
}


void render_window::_handleWindowResize (unsigned width, unsigned height)
{
    _width = width;
    _height = height;

    _sizeChangedEvent (width, height);
}


double render_window::getAspectRatio() const
{
    return _width / _height;
}


void render_window::saveScreenshot (const string &fileToSave)
{
    SOIL_save_screenshot (fileToSave.c_str(), SOIL_SAVE_TYPE_TGA, 0, 0, _width, _height);
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


void render_window::_initWindow (const string &title)
{
    if (!glfwInit())
    {
        debug::log::println_err ("failed to initialize GLFW; can't create render window");
        return;
    }

    debug::log::println ("creating rendering window with OpenGL 3.3 core context ...");
    glfwDefaultWindowHints();

    glfwWindowHint (GLFW_SAMPLES, 0);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _window = glfwCreateWindow (_width, _height, title.c_str(), nullptr, nullptr);
    if (!_window)
    {
        debug::log::println_err ("failed to create rendering window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent (_window);
}


void render_window::_handleKeyEvent (int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)  _handleKeyPressed (key);
}


void render_window::_handleKeyPressed (int key)
{
    if (key == GLFW_KEY_F12)
    {
        auto tt = time (nullptr);
        saveScreenshot (mkstr ("screenshots/screenshot-", tt, ".tga"));
    }

    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose (_window, 1);
    }

    _keyPressedEvent (key);
}
