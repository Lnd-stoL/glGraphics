
#ifndef __statistics_included__
#define __statistics_included__

//----------------------------------------------------------------------------------------------------------------------

#include "render_window.hpp"
#include "render_resources.hpp"
#include "scene.hpp"

using oo_extensions::mkstr;
using namespace render;

//----------------------------------------------------------------------------------------------------------------------

class statistics :
        public oo_extensions::non_copyable
{
    graphics_renderer &_renderer;
    render_window     &_renderWindow;
    resources         &_resources;


protected:


public:
    statistics (graphics_renderer& renderer, render_window &renderWindow, resources &res);
};

//----------------------------------------------------------------------------------------------------------------------

#endif
