
#include "demo_scene.hpp"

#include "volumetric_fog.hpp"
#include "resource_manager_impl.hpp"
#include "render_resources_impl.hpp"
#include "mesh_impl.hpp"

using oo_extensions::mkfstr;

//----------------------------------------------------------------------------------------------------------------------

demo_scene::demo_scene (graphics_renderer& renderer, render_window::ptr renderWindow, resources &res) :
    _renderer (renderer),
    _renderWindow (renderWindow),
    _resources (res)
{
    _loadAndInitialize();
}


void demo_scene::_loadAndInitialize()
{
    _setEventHandlers();

    _initViewer();
    _initShadowmaps();
    _initPosteffects();
    _initOverlays();

    _initObjects();
    _resources.releaseUnused();
}


void demo_scene::_initViewer()
{
    debug::log::println ("initializing viewer ...");

    unique_ptr<perspective_projection_d> projection (new perspective_projection_d (
            angle_d::pi / 4, _renderWindow->aspectRatio(), interval_d (1, 1000)));

    _viewerCamera = camera::alloc (move (projection));
    _viewerCamera->syncProjectionAspectRatio (_renderWindow->sizeChangedEvent());

    transform_d initialCameraTransform (vector3_d (-8, 12, 15), rotation_d (vector3_d (1, 0, 0), 0));
    _viewerCamera->addTransform (initialCameraTransform);

    _fpsCameraController = fps_camera_controller::alloc (*_renderWindow, _viewerCamera);
    _fpsCameraController->tune (_resources.requestFromFile<config_set> ("fps_camera.info"));
    _fpsCameraController->constraint ([this] (const vector3_d &newPos, const math3d::rotation_d&)
                                      {
                                          double minCameraY = _waterObject->surfaceHeight() + 1.1;
                                          return newPos.y() > minCameraY;
                                      });
}


void demo_scene::_initShadowmaps()
{
    rotation_d lightRot (vector3_d (0, 1, 0), (angle_d::pi, 0));
    lightRot.combine (rotation_d (vector3_d (1, 0, 0), (angle_d::pi / 3, 0)));

    vector2<unsigned> shadowmapSize (_renderWindow->width() * 3, _renderWindow->width() * 3);
    _shadowsRenderer = shadowmap_shadows::alloc (_resources, shadowmapSize);
    _shadowsRenderer->changeLight (vector3_f (0, 30, 80), lightRot);
}


void demo_scene::_initObjects()
{
    _scene = scene::alloc();

    auto waterReflectionsGroup = _scene->addRenderGroup ("water-reflections");
    auto shadowRecvGroup       = _scene->addRenderGroup ("shadow-recv");
    auto shadowCastGroup       = _scene->addRenderGroup ("shadow-cast");
    auto solidNoShadowGroup    = _scene->addRenderGroup ("solid-no-shadow");

    debug::log::println ("loading scene objects ...");

    auto islandMesh = _resources.requestFromFile<exs3d_mesh> ("tropical-island/tropical-island.exs3d");
    transform_d islandTransform (vector3_d (0, 0, 0), rotation_d(), vector3_d (0.08, 0.095, 0.08));
    _islandObject = mesh_renderable_object::alloc (islandMesh->renderableMesh(), islandTransform);
    islandMesh->renderableMesh()->component ("island_mesh01_3")->backfaceCulling (false);
    _scene->addRenderableObject (shadowRecvGroup, _islandObject, 1);
    _scene->addRenderableObject (waterReflectionsGroup, _islandObject, 1);
    _scene->addRenderableObject (shadowCastGroup, _islandObject, 1);

    auto rocksMesh = _resources.requestFromFile<exs3d_mesh> ("some-terrain/terrain.exs3d");
    transform_d rocksTransform (vector3_d (62, 29.3, -10), rotation_d (vector3_d (1, 0, 0), 3.14 + 0.7).combine (rotation_d (vector3_d (0, 0, -1), 0.3)), vector3_d (5, 5, 5));
    _scene->addRenderableObject (shadowRecvGroup, mesh_renderable_object::alloc (rocksMesh->renderableMesh(), rocksTransform), 1);
    _scene->addRenderableObject (shadowCastGroup, mesh_renderable_object::alloc (rocksMesh->renderableMesh(), rocksTransform), 1);

    auto stoneMesh = _resources.requestFromFile<exs3d_mesh> ("some-stone/mesh.exs3d");
    auto bumpMappingShader = _resources.gpuProgramsManager().request (gpu_program::id (exs3d_mesh::exs3d_vertex_layout::alloc(),
                                                                                       "solid_default.vert",
                                                                                       "solid_default_tangent_frame.geom",
                                                                                       "bumpmapping.frag"),
                                                                                       _resources);
    auto stoneBumpMappedMaterial = material::alloc (technique::alloc (bumpMappingShader));
    stoneBumpMappedMaterial->set ("uTxtDiffuse", _resources.requestFromFile<texture> ("models/some-stone/Stone_5_DiffuseMap.jpg"));
    stoneBumpMappedMaterial->set ("uTxtNormalmap", _resources.requestFromFile<texture> ("models/some-stone/Stone_5_LOD1NormalsMap.jpg"));
    stoneMesh->renderableMesh()->components()[0]->changeMaterial (stoneBumpMappedMaterial);
    transform_d stoneTransform (vector3_d (20, 2, 0), rotation_d(), vector3_d (0.09));
    auto stoneMeshObject = mesh_renderable_object::alloc (stoneMesh->renderableMesh(), stoneTransform);
    _scene->addRenderableObject (shadowRecvGroup, stoneMeshObject, 1);
    _scene->addRenderableObject (shadowCastGroup, stoneMeshObject, 1);

    auto cubeMesh = _resources.requestFromFile<exs3d_mesh> ("cube-textured.exs3d");
    auto parallaxMappingShader = _resources.gpuProgramsManager().request (gpu_program::id (exs3d_mesh::exs3d_vertex_layout::alloc(),
                                                                                           "solid_default.vert",
                                                                                           "solid_default_tangent_frame.geom",
                                                                                           "reliefmapping.frag"),
                                                                      _resources);
    auto parallaxMappedMaterial = material::alloc (technique::alloc (parallaxMappingShader));

    parallaxMappedMaterial->set ("uTxtDiffuse",   _resources.requestFromFile<texture> ("rockwall/rockwall.tga"));
    parallaxMappedMaterial->set ("uTxtNormalmap", _resources.requestFromFile<texture> ("rockwall/rockwall_normal.tga"));
    parallaxMappedMaterial->set ("uTxtHeightmap", _resources.requestFromFile<texture> ("rockwall/rockwall_height.dds"));

    cubeMesh->renderableMesh()->components()[0]->changeMaterial (parallaxMappedMaterial);
    transform_d cubeTransform (vector3_d (-14, 1.5, 10), rotation_d(), vector3_d (3.3, 4, 3.3));
    auto paraCubeObject = mesh_renderable_object::alloc (cubeMesh->renderableMesh(), cubeTransform);
    _scene->addRenderableObject (shadowCastGroup, paraCubeObject, 1);
    _scene->addRenderableObject (shadowRecvGroup, paraCubeObject, 1);

    _waterObject = water_plane::alloc (_resources, *_renderWindow, 1.1);
    _waterObject->useRefractionTextures (_solidSceneRT->colorTexture(), _solidSceneRT->depthTexture());

    _skies = dynamic_sky::alloc (_resources, 500);
    _scene->addRenderableObject (solidNoShadowGroup, _skies, 0);
    _scene->addRenderableObject (waterReflectionsGroup, _skies);

    _fogObject = volumetric_fog::createLayer (_resources, interval_d (2, 4), vector2_d (10, 10));
    _fogObject->useDepthTexture (_solidSceneRT->depthTexture());

    _testPath = spline_path::alloc ("resources/recorded-path.path");
    _testPath->playOnCamera (_viewerCamera, _renderWindow->frameUpdateEvent());

    _particles = particle_system_object::alloc (transform_d (vector3_d (-8, 5, -8), rotation_d()), _resources);
    _scene->addRenderableObject (_particles, 2);
}


void demo_scene::_initPosteffects()
{
    _solidSceneRT = offscreen_render_target::alloc (_renderWindow->size() * 1, 2, true);

    auto solidScenePostprocessRT = offscreen_render_target::alloc (_renderWindow->size() * 1, 1, _solidSceneRT->depthTexture());
    _solidScenePostprocess = gpu_image_processing_stage::alloc (_resources, "postprocess/ssao-1.frag", solidScenePostprocessRT);
    _solidScenePostprocess->input ("uNormalMap", _solidSceneRT->colorTextures()[1]);
    _solidScenePostprocess->input ("uScreen",    _solidSceneRT->colorTextures()[0]);
    _solidScenePostprocess->input ("uDepthMap",  _solidSceneRT->depthTexture());

    _finalPostprocess = gpu_image_processing_screen::alloc (_resources, "postprocess/fxaa_colortune.frag", _renderWindow);
    _finalPostprocess->input ("uTxtInput", _solidScenePostprocess->renderTarget()->colorTexture());
}


void demo_scene::_setEventHandlers()
{
    _renderWindow->frameUpdateEvent().handleWith ([this](double time)          { _frameUpdate (time); });
    _renderWindow->frameDrawEvent().handleWith   ([this]()                     { _frameRender();      });
    _renderWindow->keyPressedEvent().handleWith  ([this](int key)              { _keyPressed (key);   });
    _renderWindow->sizeChangedEvent().handleWith ([this](unsigned, unsigned)   { _windowResized();    });
}


void demo_scene::_frameUpdate (double time)
{
    _skies->update (time);
    _scene->setDirectionalLight (_skies->sunDirection() * _skies->skySphereRadius() * 0.9, _skies->sunColor());
    _fogObject->color (_skies->sunColor().lighten (0.1));
    _fogObject->density (_fogObject->density() * 0.998f);

    _finalPostprocess->program()->setUniform ("uGamma", 1.4 - std::sin (_skies->dayTime()) * 0.6);
    //_skies->timeMultiplyer (_skies->timeMultiplyer() * (std::cos (_skies->dayTime()) * 1.1 + 0.01));

    rotation_d lightRot (vector3_d (0, 0, 1), _skies->sunDirection().convertType<double>());
    _shadowsRenderer->changeLight (_skies->sunDirection() * _skies->skySphereRadius() * 0.2, lightRot);

    if (_viewPosLabel->isVisible())
    {
        auto viewPos = _viewerCamera->transform().ttranslation();
        string viewPosText = mkfstr (std::setprecision (4), viewPos.x(), " ", viewPos.y(), " ", viewPos.z());
        _viewPosLabel->changeText (viewPosText);
    }
}


void demo_scene::_frameRender()
{
    //if (int (_renderer.frameCount()) % 2 == 0)
        _shadowsRenderer->generateShadowmap (_renderer, _scene, _scene->renderGroup ("shadow-cast"));

    // ------------------------------------------------------------------------ reflections

    _renderer.use (_viewerCamera);
    //if (int (_renderer.frameCount()) % 2 == 0)
        _waterObject->drawReflections (_renderer, *_scene, _scene->renderGroup ("water-reflections"));

    // ------------------------------------------------------------------------ render pass

    _solidSceneRT->setup (_renderer);
    _renderer.use (_viewerCamera);
    _shadowsRenderer->drawShadedScene (_renderer, _scene, _scene->renderGroup ("shadow-recv"));
    _renderer.renderScene (_scene, _scene->renderGroup ("solid-no-shadow"));

    // ------------------------------------------------------------------------  draw water now

    _renderer.writeDepth (false);
    auto invProjMat = _viewerCamera->projection ()->asInverseMatrix();
    _solidScenePostprocess->program()->setUniform ("uMatInvProjection", invProjMat.convertType<float>());
    _solidScenePostprocess->processUsing (_renderer);

    _renderer.blend (true);
    _renderer.use (_viewerCamera);
    _waterObject->draw (_renderer);

    _particles->draw (_renderer);

    // -----------------------------------------------------------------------  draw fog now

    _fogObject->draw (_renderer);

    _renderer.writeDepth (true);
    _renderer.blend (false);

    // ----------------------------------------------------------------------  finally draw to screen

    _finalPostprocess->processUsing (_renderer);
    _statisticsOverlay->draw (_renderer);
}


void demo_scene::_initOverlays()
{
    _screenOverlay = screen_overlay_layer::alloc (_resources);
    _statisticsOverlay = statistics::alloc (*_renderWindow, _resources, _screenOverlay);

    _viewPosLabel = text_label::alloc (_statisticsOverlay->defaultFont(),
                                       math3d::vector2_f (0.34, 0.02),
                                       math3d::vector2_f (0.06, 0.06));

    _viewPosLabel->labelColor (color_rgb<float> (0.4, 0.9, 0.5));
    _viewPosLabel->hide();
    _screenOverlay->addOverlay (_viewPosLabel);

    _controlsLabel = text_label::alloc (_statisticsOverlay->defaultFont(),
                                        math3d::vector2_f (0.21, 1 - 0.08),
                                        math3d::vector2_f (0.045, 0.045));

    _controlsLabel->changeText ("press <space> to enter free camera mode");
    _controlsLabel->labelColor (color_rgb<float> (0.8, 0.8, 0.85));
    _screenOverlay->addOverlay (_controlsLabel);

    _timeControlsLabel = text_label::alloc (_statisticsOverlay->defaultFont(),
                                        math3d::vector2_f (0.18, 1 - 0.05),
                                        math3d::vector2_f (0.032, 0.032));

    _timeControlsLabel->changeText ("<8> - pause/unpause time; <9> <0> - speed up / slow down time");
    _timeControlsLabel->labelColor (color_rgb<float> (0.8, 0.8, 0.85));
    _screenOverlay->addOverlay (_timeControlsLabel);
}


void demo_scene::_keyPressed (int key)
{
    if (key == GLFW_KEY_F2)
    {
        spline_path::ptr sp = _testPathRecorder.stopRecording();
        sp->save ("resources/recorded-path.path");
    }

    if (key == GLFW_KEY_P)
    {
        if (_viewPosLabel->isVisible())  _viewPosLabel->hide();
        else                             _viewPosLabel->makeVisible();
    }

    if (key == GLFW_KEY_8)
    {
        if (_skies->timePaused())  _skies->timeContinue();
        else                       _skies->timePause();
    }

    if (key == GLFW_KEY_9)
    {
        _skies->timeMultiplyer (_skies->timeMultiplyer() * 0.75);
    }

    if (key == GLFW_KEY_0)
    {
        _skies->timeMultiplyer (_skies->timeMultiplyer() * 1.25);
    }

    if (key == GLFW_KEY_SPACE)
    {
        _testPath->stopPlaying();
        _controlsLabel->changeText ("mouse rotate  <w> <a> <s> <d> to move");
    }

    if (key == GLFW_KEY_F3)
        _testPathRecorder.recordFromCamera (_viewerCamera, _renderWindow->frameUpdateEvent());
}


void demo_scene::_windowResized()
{
    _initPosteffects();

    _waterObject->useRefractionTextures (_solidSceneRT->colorTexture(), _solidSceneRT->depthTexture());
    _fogObject->useDepthTexture (_solidSceneRT->depthTexture());
}
