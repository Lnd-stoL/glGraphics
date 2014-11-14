
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

    rotation_d lightRot (vector3_d (0, 1, 0), angle_d::pi);
    lightRot.combine (rotation_d (vector3_d (1, 0, 0), angle_d::pi / 3));
    _lightTransform = transform_d (vector3_d (10, 70, -50), lightRot);

    //unique_ptr<orthographic_projection_d> lightProj (
    //        new orthographic_projection_d (10, renderWindow.getAspectRatio(), interval_d (1, 5000)));

    unique_ptr<perspective_projection_d> lightProj (
            new perspective_projection_d (angle_d::pi / 5, _renderWindow.getAspectRatio(), interval_d (1, 100)));

    _shadowmapCamera = render::camera::alloc (std::move (lightProj));
    _shadowmapCamera->addTransform (_lightTransform);

    _shadowmapFrameBuffer = frame_buffer::alloc (_renderWindow.getWidth() * 5, _renderWindow.getHeight() * 5);
    _shadowmapTexture = _shadowmapFrameBuffer->attachDepthTexture();
    _shadowmapTexture->setupForShadowSampler();

    if (!_shadowmapFrameBuffer->readyForRender())
        debug::log::println_err ("failed to initialize frame buffer for shadowmaps");

    auto vertexLayout = shadowmapgen_vertex_layout::alloc();
    auto shadowmapGenProgramId = gpu_program::id (vertexLayout, "shadowmap_gen.vert", "shadowmap_gen.frag");
    auto shadowmapGenProgram = _resources.gpuProgramsManager().request (shadowmapGenProgramId, _resources);
    _shadowmapGenMaterial = material::alloc (technique::alloc (shadowmapGenProgram));
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
    glDisable (GL_CULL_FACE);   // TODO: So stupid model

    _renderer.renderTo (_shadowmapFrameBuffer);
    _renderer.forceMaterial (_shadowmapGenMaterial);

    _renderer.use (_shadowmapCamera);
    _islandObject->draw (_renderer);

    _renderer.stopForcingMaterial();

    // ---------------------------------------------------------------------------------------------  Render pass

    _renderer.renderTo (_renderWindow);

    auto beforeDrawLambda = [this] (graphics_renderer &renderer){
        object2screen_transform_d shadowmapTranfrom (
                renderer.state().getObject2ScreenTransform().getWorldTransform(),
                _lightTransform, _shadowmapCamera->getProjection());
        matrix_4x4_f matBias (0.5f, 0.5f, 0.5f, 1.0f);
        matBias.setCol3 (3, 0.5f, 0.5f, 0.5f);
        auto matShadowmapTransform = shadowmapTranfrom.asMatrix().convertType<float>();
        //matShadowmapTransform.multiply (matBias);
        matBias.multiply (matShadowmapTransform);
        //matBias = matShadowmapTransform;

        renderer.state().getRenderingProgram()->setUniform ("uShadowmapTransform", matBias);
    };

    auto handlerId = _renderer.beforeDrawCallEvent().handleWith (beforeDrawLambda);

    glActiveTexture (GL_TEXTURE0 + 4);
    _shadowmapTexture->use();
    glBindSampler (4, GL_LINEAR);
    _islandObject->getMesh()->getComponents()[0]->getMaterial()->getTechnique()->getRenderingProgram()->setUniformSampler ("uShadowMap", 4, true);
    _islandObject->getMesh()->getComponents()[0]->getMaterial()->getTechnique()->getRenderingProgram()->setUniformSampler ("uShadowMapFlat", 4, true);
    _islandObject->getMesh()->getComponents()[0]->getMaterial()->getTechnique()->getRenderingProgram()->setUniform ("uLightPos", _lightTransform.getTranslation().convertType<float>());

    glActiveTexture (GL_TEXTURE0);

    _renderer.use (_viewerCamera);
    _islandObject->draw (_renderer);

    _renderer.beforeDrawCallEvent().stopHandlingWith (handlerId);
}
