
#include "demo_scene.hpp"

using oo_extensions::mkstr;
using namespace math3D;

//----------------------------------------------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    debug::log::println ("initializing OpenGL rendering window ...");

    render_window renderWindow (1200, 900, "OpenGL Graphics");
    graphics_renderer renderer (renderWindow);
    resources resourceManagers;

    demo_scene demoScene (renderer, renderWindow, resourceManagers);

    debug::log::println ("====== rendering  started ======");
    renderWindow.runEventLoop();
    debug::log::println ("====== rendering finished ======");

    return 0;
}
