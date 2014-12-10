
#include "demo_scene.hpp"

#include "resource_manager_impl.hpp"

using oo_extensions::mkstr;
using namespace math3d;

//----------------------------------------------------------------------------------------------------------------------

class application
{
    resources  _resourceManagers;
    render_window::ptr  _renderWindow;

    unique_ptr<graphics_renderer>  _graphicsRenderer;
    unique_ptr<demo_scene>  _demoScene;


protected:
    void _initializeResources()
    {
        debug::log::println ("initializing resource managers ...");

        _resourceManagers.configFilesManager().addFileSearchLocation ("config");
        _resourceManagers.exs3dMeshesManager().addFileSearchLocation ("resources/models");
        _resourceManagers.gpuProgramsManager().addFileSearchLocation ("resources/shaders");
        _resourceManagers.vertexShadersManager().addFileSearchLocation ("resources/shaders");
        _resourceManagers.fragmentShadersManager().addFileSearchLocation ("resources/shaders");
        _resourceManagers.geometryShadersManager().addFileSearchLocation ("resources/shaders");
        _resourceManagers.fontsManager().addFileSearchLocation ("resources/fonts");
        _resourceManagers.texturesManager().addFileSearchLocation ("resources/textures");

        _resourceManagers.addFilesSearchLocation ("resources");
    }


    void _initializeRenderWindow()
    {
        config_set::ptr renderWindowConfig =
                _resourceManagers.requestFromFile<config_set> ("render_window.info")->getGroup ("render_window");

        debug::log::println ("initializing OpenGL rendering window ...");
        _renderWindow = render_window::create (renderWindowConfig->get<unsigned> ("width"),
                                               renderWindowConfig->get<unsigned> ("height"),
                                               renderWindowConfig->get<string>   ("title"),
                                               renderWindowConfig->get<bool>     ("fullscreen"));
    }


public:
    application()
    {
        _initializeResources();
        _initializeRenderWindow();

        _graphicsRenderer = unique_ptr<graphics_renderer> (new graphics_renderer (_renderWindow));
        _demoScene = unique_ptr<demo_scene> (new demo_scene (*_graphicsRenderer, _renderWindow, _resourceManagers));
    }


    void run()
    {
        debug::log::println ("====== rendering  started ======");
        _renderWindow->runLoop();
        debug::log::println ("====== rendering finished ======");
    }
};

//----------------------------------------------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    try
    {
        application app;
        app.run();
    }

    catch (const debug::gl_exception &except)
    {
        debug::log::println_err (mkstr ("fatal gl_exception: ", except.what()));
    }

    catch (const std::exception &except)
    {
        debug::log::println_err (mkstr ("fatal exception: ", except.what()));
    }

    return 0;
}
