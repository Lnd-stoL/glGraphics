
#ifndef __render_window__included__
#define __render_window__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <functional>

using oo_extensions::event;

//----------------------------------------------------------------------------------------------------------------------

class render_window
{
    sf::RenderWindow _window;
    event<void, const render_window&> _frameUpdateEvent;
    event<void, const render_window&> _frameDrawEvent;

public:
    property_get_ref (Window, _window)
    event_access (frameUpdate, _frameUpdateEvent)
    event_access (frameDraw,   _frameDrawEvent)


public:
    render_window (unsigned width, unsigned height, const string &title);
    void runEventLoop();
};

//----------------------------------------------------------------------------------------------------------------------

#endif


