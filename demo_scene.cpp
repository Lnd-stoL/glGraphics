
#include "demo_scene.hpp"

#include "resource_manager_impl.hpp"
#include "render_resources_impl.hpp"

//----------------------------------------------------------------------------------------------------------------------

demo_scene::demo_scene (graphics_renderer& renderer, render_window &renderWindow, resources &res) :
        _renderer (renderer),
        _renderWindow (renderWindow),
        _resources (res)
{
    _loadAndInitialize();
}


void demo_scene::_loadAndInitialize()
{
    _setEventHandlers();
    _initResourceManagers();

    _initViewer();
    _initObjects();

    _initShadowmaps();
}


void demo_scene::_initViewer()
{
    debug::log::println ("initializing viewer ...");

    unique_ptr<perspective_projection_d> projection (new perspective_projection_d (
            angle_d::pi / 4, _renderWindow.getAspectRatio(), interval_d (1, 1000)));

    _viewerCamera = camera::alloc (move (projection));
    _viewerCamera->syncProjectionAspectRatio (_renderWindow.sizeChangedEvent());

    transform_d initialCameraTransform (vector3_d (0, 8, 0), rotation_d (vector3_d (1, 0, 0), 0));
    _viewerCamera->addTransform (initialCameraTransform);

    _fpsCameraController = fps_camera_controller::alloc (_renderWindow, _viewerCamera);
}


void demo_scene::_initShadowmaps()
{
    debug::log::println ("initializing shadowmap stuff ...");

    _lightTransform = transform_d (vector3_d (0, 50, 50), rotation_d (vector3_d (1, 0, 0), angle_d::pi / 4));

    //unique_ptr<orthographic_projection_d> lightProj (
    //        new orthographic_projection_d (10, renderWindow.getAspectRatio(), interval_d (1, 5000)));

    unique_ptr<perspective_projection_d> lightProj (
            new perspective_projection_d (angle_d::pi / 4, _renderWindow.getAspectRatio(), interval_d (1, 200)));

    _shadowmapCamera = render::camera::alloc (std::move (lightProj));
    _shadowmapCamera->addTransform (_lightTransform);

    _shadowmapFrameBuffer = frame_buffer::alloc (_renderWindow.getWidth() * 3, _renderWindow.getHeight() * 3);
    _shadowmapTexture = _shadowmapFrameBuffer->attachDepthTexture();

    if (!_shadowmapFrameBuffer->readyForRender())
        debug::log::println_err ("failed to initialize frame buffer for shadowmaps");
}


void demo_scene::_initResourceManagers()
{
    _resources.exs3dMeshesManager().addFileSearchLocation ("resources/models");
    _resources.gpuProgramsManager().addFileSearchLocation ("resources/shaders");
    _resources.fontsManager().addFileSearchLocation ("resources/fonts");
}


void demo_scene::_initObjects()
{
    debug::log::println ("loading scene objects ...");

    auto islandMesh = _resources.requestFromFile<exs3d_mesh> ("tropical-island/tropical-island.exs3d");

    transform_d islandTransform (vector3_d (0, 0, 0), rotation_d(), vector3_d (0.04));
    _islandObject = mesh_renderable_object::alloc (islandMesh->getRenderableMesh(), islandTransform);
}


void demo_scene::_setEventHandlers()
{
    _renderWindow.frameUpdateEvent().handleWith ([this](const render_window&) { _frameUpdate(); });
    _renderWindow.frameDrawEvent().handleWith   ([this](const render_window&) { _frameRender(); });
}


void demo_scene::_frameUpdate()
{

}


void demo_scene::_frameRender()
{
    _shadowmapFrameBuffer->use();
    glViewport (0, 0, _shadowmapTexture->getWidth(), _shadowmapTexture->getHeight());

    glDrawBuffer (GL_NONE);
    glReadBuffer (GL_NONE);

    glClearColor (1, 1, 1, 1);
    glClear (GL_DEPTH_BUFFER_BIT);

    object2screen_transform_d shadowMapTranfrom (_islandObject->getTransform(), _lightTransform, _shadowmapCamera->getProjection());
    //matrix_4x4_f matBias (0.5f, 0.5f, 0.5f, 1.0f);
    //matBias.setRow3 (3, 0.5f, 0.5f, 0.5f, 0.5f);
    auto matShadow = shadowMapTranfrom.asMatrix().convertType<float>();
    //matBias.multiply (matShadow);

    //sceneObj2->getMesh()->getComponents()[0]->getMaterial()->getRenderingProgram()->setUniform ("uShadowmapTransform", matBias);

    _renderer.state().changeCamera (_shadowmapCamera);
    _islandObject->draw (_renderer);

    // ---------------------------------------------------------------------------------------------  Render pass

    frame_buffer::useDefault();
    glViewport (0, 0, _renderWindow.getWidth(), _renderWindow.getHeight());

    glDrawBuffer (GL_BACK);
    glReadBuffer (GL_BACK);

    glClearColor (1, 1, 1, 1);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _islandObject->getMesh()->getComponents()[0]->getMaterial()->getTechnique()->getRenderingProgram()->setUniform ("uShadowmapTransform", matShadow, true);
    glActiveTexture (GL_TEXTURE0 + 4);
    _shadowmapTexture->use();
    glBindSampler (4, GL_LINEAR);

    _islandObject->getMesh()->getComponents()[0]->getMaterial()->getTechnique()->getRenderingProgram()->setUniformSampler ("uShadowMap", 4, true);
    glActiveTexture (GL_TEXTURE0);

    _renderer.state().changeCamera (_viewerCamera);
    _islandObject->draw (_renderer);

    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}
