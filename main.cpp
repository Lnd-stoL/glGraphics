
#include "demo_scene.hpp"

using oo_extensions::mkstr;
using namespace math3d;

//----------------------------------------------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    debug::log::println ("initializing OpenGL rendering window ...");

    render_window renderWindow (1400, 1000, "OpenGL graphics tech demo (by Leonid Stolyarov aka Lnd-stoL)");
    graphics_renderer renderer (renderWindow);
    resources resourceManagers;

    demo_scene demoScene (renderer, renderWindow, resourceManagers);

    debug::log::println ("====== rendering  started ======");
    renderWindow.runEventLoop();
    debug::log::println ("====== rendering finished ======");

    return 0;
}
