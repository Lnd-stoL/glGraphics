
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
#include "sky_box.hpp"
#include "statistics.hpp"
#include "volumetric_fog.hpp"
#include "spline_path.hpp"
#include "particles_system.hpp"
#include "gpu_image_processing.hpp"

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

    offscreen_render_target::ptr      _shadowmapRT;
    offscreen_render_target::ptr      _solidSceneRT;
    gpu_image_processing_stage::ptr   _solidScenePostprocess;
    gpu_image_processing_screen::ptr  _finalPostprocess;

    camera::ptr       _shadowmapCamera;
    transform_d       _lightTransform;
    material::ptr     _shadowmapGenMaterial;

    scene::ptr  _scene;
    mesh_renderable_object::ptr  _islandObject;
    water_plane::ptr  _waterObject;
    sky_box::ptr  _skyBox;
    float _time = 0;
    math3d::vector3_f _sunPosition;
    sf::Image  _horizonColorMap;
    volumetric_fog::ptr  _fogObject;
    particle_system_object::ptr  _particles;

    screen_overlay_layer::ptr  _screenOverlay;
    statistics::ptr  _statisticsOverlay;
    text_label::ptr  _viewPosLabel;
    spline_path::ptr _testPath;
    spline_path_recorder _testPathRecorder;


protected:
    void _loadAndInitialize();
    void _setEventHandlers();

    void _initViewer();
    void _initShadowmaps();
    void _initObjects();
    void _initPosteffects();
    void _initOverlays();

    void _frameUpdate();
    void _frameRender();
    void _keyPressed (int key);


public:
    demo_scene (graphics_renderer& renderer, render_window::ptr renderWindow, resources &res);
};

//----------------------------------------------------------------------------------------------------------------------

struct shadowmapgen_vertex_layout : vertex_layout<exs3d_mesh::vertex>
{
protected:
    virtual void _registerAttributes()
    {
        _attributes.clear();
        _registerAttribute ("aCoords", attribute::tFloat, offsetof (exs3d_mesh::vertex, coords), 3);
    }

public:
    declare_ptr_alloc (shadowmapgen_vertex_layout)
    shadowmapgen_vertex_layout()  { _registerAttributes(); }
};

//----------------------------------------------------------------------------------------------------------------------

#endif
