
#ifndef __render_window__included__
#define __render_window__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <functional>

using oo_extensions::event;

//----------------------------------------------------------------------------------------------------------------------

class render_window :
    public oo_extensions::non_copyable
{
private:
    static weak_ptr<render_window>  _singleton;

    GLFWwindow *_window = nullptr;
    unsigned _width, _height;

    static constexpr unsigned _avgMaxSamplesCount = 10000;
    double  _frameTime        = 0;
    double  _avgFrameTime     = 0;
    double  _frameTimeSamples = 0;
    double  _lastUpdateTime   = 0;

    event<double>               _frameUpdateEvent;
    event<>                     _frameDrawEvent;
    event<unsigned, unsigned>   _sizeChangedEvent;
    event<int>                  _keyPressedEvent;


public:
    event_access (frameUpdate, _frameUpdateEvent)
    event_access (frameDraw,   _frameDrawEvent)
    event_access (sizeChanged, _sizeChangedEvent)
    event_access (keyPressed,  _keyPressedEvent)

    property_get (frameTime,        _frameTime)
    property_get (averageFrameTime, _avgFrameTime / _frameTimeSamples)


private:
    render_window (unsigned width, unsigned height, const string &title, bool fullscreen = false);

    void _handleWindowResize (unsigned width, unsigned height);
    void _handleKeyEvent (int key, int scancode, int action, int mods);
    void _handleKeyPressed (int key);
    void _initWindow (const string &title, bool fullscreen);

    static void _windowSizeCallback (GLFWwindow* window, int width, int height);
    static void _keyboardCallback (GLFWwindow* window, int key, int scancode, int action, int mods);


public:
    declare_ptr (render_window)
    static render_window::ptr create (unsigned width, unsigned height, const string &title, bool fullscreen = false);

    void runLoop();
    void saveScreenshot (const string &fileToSave);

    void clear();

    unsigned width()  const;
    unsigned height() const;
    math3d::vector2<unsigned> size() const;

    double aspectRatio() const;
};

//----------------------------------------------------------------------------------------------------------------------

#endif
