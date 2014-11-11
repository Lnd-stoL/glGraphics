
#ifndef __demo_scene_included__
#define __demo_scene_included__

//----------------------------------------------------------------------------------------------------------------------

#include "render_window.hpp"
#include "fps_camera.hpp"
#include "scene.hpp"
#include "mesh_renderable_object.hpp"
#include "frame_buffer.hpp"
#include "render_resources.hpp"

using oo_extensions::mkstr;
using namespace math3D;
using namespace render;

//----------------------------------------------------------------------------------------------------------------------

class demo_scene :
    public oo_extensions::non_copyable
{
    graphics_renderer &_renderer;
    render_window     &_renderWindow;
    resources         &_resources;

    camera::ptr    _viewerCamera;
    fps_camera_controller::ptr _fpsCameraController;

    camera::ptr       _shadowmapCamera;
    transform_d       _lightTransform;
    frame_buffer::ptr _shadowmapFrameBuffer;
    texture::ptr      _shadowmapTexture;

    mesh_renderable_object::ptr _islandObject;


protected:
    void _loadAndInitialize();
    void _setEventHandlers();

    void _initResourceManagers();
    void _initViewer();
    void _initShadowmaps();
    void _initObjects();

    void _frameUpdate();
    void _frameRender();


public:
    demo_scene (graphics_renderer& renderer, render_window &renderWindow, resources &res);
};

//----------------------------------------------------------------------------------------------------------------------

#endif
