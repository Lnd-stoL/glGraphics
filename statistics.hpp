
#ifndef __statistics_included__
#define __statistics_included__

//----------------------------------------------------------------------------------------------------------------------

#include "render_window.hpp"
#include "render_resources.hpp"
#include "screen_overlay.hpp"

using oo_extensions::mkstr;
using namespace render;

//----------------------------------------------------------------------------------------------------------------------

class statistics :
    public oo_extensions::non_copyable
{
    render_window  &_renderWindow;
    resources      &_resources;

    font::ptr  _defaultFont;
    screen_overlay_layer::ptr  _overlayLayer;
    unsigned  _updatePeriodFrames = 15;

    text_label::ptr  _frameTimeLabel;
    text_label::ptr  _avgFrameTimeLabel;
    text_label::ptr  _renderLabel;
    text_label::ptr  _trianglesLabel;

public:
    property_get (DefaultFont, _defaultFont)


public:
    declare_ptr_alloc (statistics)
    statistics (render_window &renderWindow, resources &res, screen_overlay_layer::ptr overlayLayer);

    void draw (graphics_renderer &renderer);
};

//----------------------------------------------------------------------------------------------------------------------

#endif
