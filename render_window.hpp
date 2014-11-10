
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

class render_window :
    public oo_extensions::non_copyable
{
    sf::RenderWindow _window;

    event<const render_window&> _frameUpdateEvent;
    event<const render_window&> _frameDrawEvent;
    event<unsigned, unsigned>   _sizeChangedEvent;


public:
    property_ref (sfmlWindow, _window)
    property_get_ref (SfmlWindow, _window)

    event_access (frameUpdate, _frameUpdateEvent)
    event_access (frameDraw,   _frameDrawEvent)
    event_access (sizeChanged, _sizeChangedEvent)


private:
    void _handleWindowResize (unsigned width, unsigned height);

public:
    declare_ptr_alloc (render_window)

    render_window (unsigned width, unsigned height, const string &title);
    void runEventLoop();
    void saveScreenshot (const string &fileToSave);

    unsigned getWidth()  const;
    unsigned getHeight() const;

    double getAspectRatio() const;
};

//----------------------------------------------------------------------------------------------------------------------

#endif


