
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

    transform_d initialCameraTransform (vector3_d (0, 8, 0), rotation_d (vector3_d (1, 0, 0), 0));
    _viewerCamera->addTransform (initialCameraTransform);

    _fpsCameraController = fps_camera_controller::alloc (*_renderWindow, _viewerCamera);
}


void demo_scene::_initShadowmaps()
{
    debug::log::println ("initializing shadowmap stuff ...");

    rotation_d lightRot (vector3_d (0, 1, 0), (angle_d::pi, 0));
    lightRot.combine (rotation_d (vector3_d (1, 0, 0), (angle_d::pi / 3, 0)));
    _lightTransform = transform_d (vector3_d (0, 30, 80), lightRot);

    vector2<unsigned> shadowmapSize (_renderWindow->width() * 3, _renderWindow->width() * 3);
    _shadowsRenderer = shadowmap_shadows::alloc (_resources, shadowmapSize);
    _shadowsRenderer->changeLight (_lightTransform.ttranslation(), _lightTransform.trotation());
}


void demo_scene::_initObjects()
{
    _scene = scene::alloc();
    _scene->setSun (_lightTransform.ttranslation(), color_rgb<float> (1, 1, 1));

    auto waterReflectionsGroup = _scene->addRenderGroup ("water-reflections");
    auto shadowRecvGroup = _scene->addRenderGroup ("shadow-recv");
    auto shadowCastGroup = _scene->addRenderGroup ("shadow-cast");
    auto solidNoShadowGroup = _scene->addRenderGroup ("solid-no-shadow");

    debug::log::println ("loading scene objects ...");

    auto islandMesh = _resources.requestFromFile<exs3d_mesh> ("tropical-island/tropical-island.exs3d");
    //auto islandMesh = _resources.requestFromFile<exs3d_mesh> ("some-terrain/terrain.exs3d");
    //transform_d islandTransform (vector3_d (0, 23, 0), rotation_d (vector3_d (1, 0, 0), 3.14 + 0.8).combine (rotation_d (vector3_d (0, 0, -1), 0.2)), vector3_d (3, 3, 3));
    transform_d islandTransform (vector3_d (0, 0, 0), rotation_d(), vector3_d (0.05));
    _islandObject = mesh_renderable_object::alloc (islandMesh->renderableMesh(), islandTransform);
    islandMesh->renderableMesh()->component ("island_mesh01_3")->backfaceCulling (false);
    _scene->addRenderableObject (shadowRecvGroup, _islandObject, 1);
    _scene->addRenderableObject (waterReflectionsGroup, _islandObject, 1);
    _scene->addRenderableObject (shadowCastGroup, _islandObject, 1);

    auto rocksMesh = _resources.requestFromFile<exs3d_mesh> ("some-terrain/terrain.exs3d");
    transform_d rocksTransform (vector3_d (45, 15, 0), rotation_d (vector3_d (1, 0, 0), 3.14 + 0.9).combine (rotation_d (vector3_d (0, 0, -1), 0.3)), vector3_d (2, 3.5, 2));
    _scene->addRenderableObject (shadowRecvGroup, mesh_renderable_object::alloc (rocksMesh->renderableMesh(), rocksTransform), 1);

    auto stoneMesh = _resources.requestFromFile<exs3d_mesh> ("some-stone/mesh.exs3d");
    auto bumpMappingShader = _resources.gpuProgramsManager().request (gpu_program::id (exs3d_mesh::exs3d_vertex_layout::alloc(),
                                                                                       "shader.vert", "tangent_frame.geom", "bumpmapping.frag"),
                                                                                       _resources);
    auto stoneBumpMappedMaterial = material::alloc (technique::alloc (bumpMappingShader));
    stoneBumpMappedMaterial->set ("uTexture", _resources.requestFromFile<texture> ("models/some-stone/Stone_5_DiffuseMap.jpg"));
    stoneBumpMappedMaterial->set ("uNormalmap", _resources.requestFromFile<texture> ("models/some-stone/Stone_5_LOD1NormalsMap.jpg"));
    stoneBumpMappedMaterial->set ("uHeightmap", _resources.requestFromFile<texture> ("models/some-stone/heightmap.jpg"));
    stoneMesh->renderableMesh()->components()[0]->changeMaterial (stoneBumpMappedMaterial);
    transform_d stoneTransform (vector3_d (14, 2, 0), rotation_d(), vector3_d (0.05));
    auto stoneMeshObject = mesh_renderable_object::alloc (stoneMesh->renderableMesh(), stoneTransform);
    _scene->addRenderableObject (shadowRecvGroup, stoneMeshObject, 1);
    _scene->addRenderableObject (shadowCastGroup, stoneMeshObject, 1);

    auto cubeMesh = _resources.requestFromFile<exs3d_mesh> ("cube-textured.exs3d");
    auto parallaxMappingShader = _resources.gpuProgramsManager().request (gpu_program::id (exs3d_mesh::exs3d_vertex_layout::alloc(),
                                                                                       "shader.vert", "tangent_frame.geom", "bumpmapping.frag"),
                                                                      _resources);
    auto parallaxMappedMaterial = material::alloc (technique::alloc (parallaxMappingShader));
    //parallaxMappedMaterial->textures()["uTexture"]   = _resources.requestFromFile<texture> ("rockwall/relief_wood.jpg");
    //parallaxMappedMaterial->textures()["uNormalmap"] = _resources.requestFromFile<texture> ("rockwall/relief_bump.png");
    //parallaxMappedMaterial->textures()["uHeightmap"] = _resources.requestFromFile<texture> ("rockwall/relief_height.dds");

    parallaxMappedMaterial->set ("uTexture", _resources.requestFromFile<texture> ("rockwall/rockwall.tga"));
    parallaxMappedMaterial->set ("uNormalmap", _resources.requestFromFile<texture> ("rockwall/rockwall_normal.tga"));
    parallaxMappedMaterial->set ("uHeightmap", _resources.requestFromFile<texture> ("rockwall/rockwall_height.dds"));

    cubeMesh->renderableMesh()->components()[0]->changeMaterial (parallaxMappedMaterial);
    transform_d cubeTransform (vector3_d (-14, 2, 0), rotation_d(), vector3_d (1));
    auto paraCubeObject = mesh_renderable_object::alloc (cubeMesh->renderableMesh(), cubeTransform);
    _scene->addRenderableObject (solidNoShadowGroup, paraCubeObject, 1);

    //auto island2Mesh = _resources.requestFromFile<exs3d_mesh> ("island-001/island.exs3d");
    //transform_d island2Transform (vector3_d (0, 0, 0), rotation_d(), vector3_d (0.04));
    //auto island2Object = mesh_renderable_object::alloc (island2Mesh->getRenderableMesh(), island2Transform);
    //_scene->addRenderableObject (island2Object, 1);

    _waterObject = water_plane::alloc (_resources, *_renderWindow, 1);
    _waterObject->useRefractionTextures (_solidSceneRT->colorTexture(), _solidSceneRT->depthTexture());

    _skyBox = sky_box::alloc (_resources);
    _scene->addRenderableObject (solidNoShadowGroup, _skyBox, 0);
    _scene->addRenderableObject (waterReflectionsGroup, _skyBox);

    _fogObject = volumetric_fog::createLayer (_resources, interval_d (0, 2.9), vector2_d (10, 10));
    //_fogObject->useDepthTexture (_sceneWithoutWater_DepthTexture);
    //_fogObject->useColorTexture (_sceneWithoutWater_Texture);

    _testPath = spline_path::alloc ("resources/recorded-path.path");
    //_testPath->playOnCamera (_viewerCamera, _renderWindow.frameUpdateEvent());
    //_testPathRecorder.recordFromCamera (_viewerCamera, _renderWindow.frameUpdateEvent());

    _particles = particle_system_object::alloc (transform_d (vector3_d (-8, 5, -8), rotation_d()), _resources);
    _scene->addRenderableObject (_particles, 2);

    _horizonColorMap.loadFromFile (_resources.texturesManager().locateFile ("skybox/horizon.png"));
}


void demo_scene::_initPosteffects()
{
    _solidSceneRT = offscreen_render_target::alloc (_renderWindow->size(), 2, true);

    auto solidScenePostprocessRT = offscreen_render_target::alloc (_renderWindow->size(), 1, _solidSceneRT->depthTexture());
    _solidScenePostprocess = gpu_image_processing_stage::alloc (_resources, "ssao-1.frag", solidScenePostprocessRT);
    _solidScenePostprocess->input ("uNormalMap", _solidSceneRT->colorTextures()[1]);
    _solidScenePostprocess->input ("uScreen",    _solidSceneRT->colorTextures()[0]);
    _solidScenePostprocess->input ("uDepthMap",  _solidSceneRT->depthTexture());

    _finalPostprocess = gpu_image_processing_screen::alloc (_resources, "fxaa.frag", _renderWindow);
    _finalPostprocess->input ("uTxtInput", _solidScenePostprocess->renderTarget()->colorTexture());
}


void demo_scene::_setEventHandlers()
{
    _renderWindow->frameUpdateEvent().handleWith ([this](const render_window&) { _frameUpdate();     });
    _renderWindow->frameDrawEvent().handleWith   ([this](const render_window&) { _frameRender();    });
    _renderWindow->keyPressedEvent().handleWith  ([this](int key)              { _keyPressed (key); });
}


void demo_scene::_frameUpdate()
{
    _time += 0.002;

    _sunPosition = math3d::vector3_f (0, std::sin (_time), std::cos (_time)).normalized();

    double sunColorMapSampleX = (2.0 * _time / angle_d::pi);
    if (sunColorMapSampleX >= 1.0)  sunColorMapSampleX = 2.01 - sunColorMapSampleX;
    unsigned sunColorMapCoordX = (unsigned) (sunColorMapSampleX  * _horizonColorMap.getSize().x);
    if (sunColorMapCoordX >= _horizonColorMap.getSize().x)  sunColorMapCoordX = _horizonColorMap.getSize().x - 1;

    auto sunColor = color_rgb<float> (_horizonColorMap.getPixel (sunColorMapCoordX, 0));

    _skyBox->update (_sunPosition);
    _scene->setSun (_sunPosition.convertType<double>() * 450, sunColor);

    rotation_d lightRot (vector3_d (0, 0, 1), _sunPosition.convertType<double>());
    auto shadowmapCameraPos = _scene->sunPosition() / 4;
    _shadowsRenderer->changeLight (shadowmapCameraPos, lightRot);

    if (_viewPosLabel->isVisible())
    {
        auto viewPos = _viewerCamera->transform().ttranslation();

        string viewPosText = mkfstr (std::setprecision (4), viewPos.x(), " ", viewPos.y(), " ", viewPos.z());
        _viewPosLabel->changeText (viewPosText);
    }

    double minCameraY = _waterObject->surfaeHight() + 1.5;
    if (_viewerCamera->position().y() < minCameraY)
    {
        auto cameraPos = _viewerCamera->position();
        cameraPos.y (minCameraY);
        _viewerCamera->changePosition (cameraPos);
    }
}


void demo_scene::_frameRender()
{
    _shadowsRenderer->generateShadowmap (_renderer, _scene, _scene->renderGroup ("shadow-cast"));

    // ---------------------------------------------------------------------------------------------  Render pass

    _solidSceneRT->setup (_renderer);
    _renderer.use (_viewerCamera);
    _shadowsRenderer->drawShadedScene (_renderer, _scene, _scene->renderGroup ("shadow-recv"));
    _renderer.renderScene (_scene, _scene->renderGroup ("solid-no-shadow"));

    // ---------------------------------------------------------------------------------------------  Reflections

    _waterObject->drawReflections (_renderer, *_scene, _scene->renderGroup ("water-reflections"));

    // ---------------------------------------------------------------------------------------------  Draw water now

    glDepthMask (GL_FALSE);
    auto invProjMat = _viewerCamera->projection ()->asInverseMatrix();
    _solidScenePostprocess->program()->setUniform ("uMatInvProjection", invProjMat.convertType<float>());
    _solidScenePostprocess->processUsing (_renderer);


    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _renderer.use (_viewerCamera);
    _waterObject->draw (_renderer);

    glDepthMask (GL_TRUE);
    glDisable (GL_BLEND);

    // ---------------------------------------------------------------------------------------------  Finally draw to screen

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
}
