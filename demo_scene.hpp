
#ifndef __demo_scene_included__
#define __demo_scene_included__

//----------------------------------------------------------------------------------------------------------------------

#include "render_window.hpp"
#include "fps_camera.hpp"
#include "scene.hpp"
#include "mesh_renderable_object.hpp"
#include "frame_buffer.hpp"
#include "render_resources.hpp"
#include "water_plane.hpp"
#include "dynamic_sky.hpp"
#include "statistics.hpp"
#include "volumetric_fog.hpp"
#include "spline_path.hpp"
#include "particles_system.hpp"
#include "gpu_image_processing.hpp"
#include "shadowmap_shadows.hpp"

using oo_extensions::mkstr;
using namespace math3d;
using namespace render;

//----------------------------------------------------------------------------------------------------------------------

class demo_scene :
    public oo_extensions::non_copyable
{
    graphics_renderer  &_renderer;
    resources          &_resources;
    render_window::ptr  _renderWindow;

    camera::ptr  _viewerCamera;
    fps_camera_controller::ptr  _fpsCameraController;

    shadowmap_shadows::ptr  _shadowsRenderer;
    offscreen_render_target::ptr      _solidSceneRT;
    gpu_image_processing_stage::ptr   _solidScenePostprocess;
    gpu_image_processing_screen::ptr  _finalPostprocess;

    scene::ptr  _scene;
    mesh_renderable_object::ptr  _islandObject;
    water_plane::ptr  _waterObject;
    dynamic_sky::ptr _skies;
    volumetric_fog::ptr  _fogObject;
    particle_system_object::ptr  _particles;

    screen_overlay_layer::ptr  _screenOverlay;
    statistics::ptr  _statisticsOverlay;
    text_label::ptr  _viewPosLabel;
    text_label::ptr  _controlsLabel;
    text_label::ptr  _timeControlsLabel;
    spline_path::ptr _testPath;
    spline_path_recorder _testPathRecorder;


protected:
    void _loadAndInitialize();
    void _setEventHandlers();

    void _initViewer();
    void _initShadowmaps();
    void _initObjects();
    void _initRenderTargets ();
    void _initOverlays();

    void _frameUpdate (double time);
    void _frameRender();
    void _keyPressed (int key);
    void _windowResized();


public:
    demo_scene (graphics_renderer& renderer, render_window::ptr renderWindow, resources &res);
};

//----------------------------------------------------------------------------------------------------------------------

#endif
