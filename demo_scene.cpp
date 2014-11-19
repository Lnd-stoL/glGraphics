
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
    _initShadowmaps();
    _initPosteffects();

    _initObjects();
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

    _shadowmapFrameBuffer = frame_buffer::alloc (_renderWindow.getWidth() * 2, _renderWindow.getWidth() * 2);
    _shadowmapTexture = _shadowmapFrameBuffer->attachDepthTexture();
    _shadowmapTexture->setupForShadowSampler();

    if (!_shadowmapFrameBuffer->readyForRender())
        debug::log::println_err ("failed to initialize frame buffer for shadowmaps");

    auto vertexLayout = shadowmapgen_vertex_layout::alloc();
    //auto vertexLayout = exs3d_mesh::exs3d_vertex_layout::alloc();
    auto shadowmapGenProgramId = gpu_program::id (vertexLayout, "shadowmap_gen.vert", "shadowmap_gen.frag");
    auto shadowmapGenProgram = _resources.gpuProgramsManager().request (shadowmapGenProgramId, _resources);
    _shadowmapGenMaterial = material::alloc (technique::alloc (shadowmapGenProgram));
}


void demo_scene::_initResourceManagers()
{
    _resources.exs3dMeshesManager().addFileSearchLocation ("resources/models");
    _resources.gpuProgramsManager().addFileSearchLocation ("resources/shaders");
    _resources.fontsManager().addFileSearchLocation ("resources/fonts");
    _resources.texturesManager().addFileSearchLocation ("resources/textures");
}


void demo_scene::_initObjects()
{
    _scene = scene::alloc();
    _scene->setSun (_lightTransform.getTranslation());

    debug::log::println ("loading scene objects ...");

    auto islandMesh = _resources.requestFromFile<exs3d_mesh> ("tropical-island/tropical-island.exs3d");
    transform_d islandTransform (vector3_d (0, 0, 0), rotation_d(), vector3_d (0.04));
    _islandObject = mesh_renderable_object::alloc (islandMesh->getRenderableMesh(), islandTransform);
    _scene->addRenderableObject (_islandObject, 1);

    _waterObject = water_plane::alloc (_resources, _renderWindow, 1);
    _waterObject->useRefractionTextures (_sceneWithoutWater_Texture, _sceneWithoutWater_DepthTexture);

    _skyBox = sky_box::alloc (_resources);
    _scene->addRenderableObject (_skyBox, 0);
}


void demo_scene::_initPosteffects()
{
    _sceneWithoutWater_FrameBuffer = frame_buffer::alloc (_renderWindow.getWidth(), _renderWindow.getHeight());
    _sceneWithoutWater_FrameBuffer->clearColor (color_rgb<float> (1, 1, 1));
    _sceneWithoutWater_DepthTexture = _sceneWithoutWater_FrameBuffer->attachDepthTexture();
    _sceneWithoutWater_Texture = _sceneWithoutWater_FrameBuffer->attachColorTexture();
    _sceneWithoutWater_NormalMap = _sceneWithoutWater_FrameBuffer->attachColorTexture();

    if (!_sceneWithoutWater_FrameBuffer->readyForRender())
        debug::log::println_err ("failed to initialize frame buffer for scene without refractive rendering");

    auto screenQuadShaderId = gpu_program::id (elementary_shapes::simple_vertex_layout::alloc(),
                                               "ssao.vert", "ssao.frag");
    auto screenQuadShader = _resources.gpuProgramsManager().request (screenQuadShaderId, _resources);
    auto screenQuadTechnique = technique::alloc (screenQuadShader);
    screenQuadTechnique->transformNotNeeded();
    _drawScreenMaterial = material::alloc (screenQuadTechnique);
    _drawScreenMaterial->textures()["uNormalMap"] = _sceneWithoutWater_NormalMap;
    _drawScreenMaterial->textures()["uScreen"] = _sceneWithoutWater_Texture;
    _drawScreenMaterial->textures()["uDepthMap"] = _sceneWithoutWater_DepthTexture;

    vector<elementary_shapes::simple_vertex> vertices;
    vector<unsigned short> indices;
    elementary_shapes::quadXY (vertices, indices);
    _screenQuad = mesh_component<elementary_shapes::simple_vertex, unsigned short>::alloc (_drawScreenMaterial,
                                                                                           vertices, indices);

    _postprocess_FrameBuffer = frame_buffer::alloc (_renderWindow.getWidth(), _renderWindow.getHeight());
    _postprocess_FrameBuffer->attachDepthTexture (_sceneWithoutWater_DepthTexture);
    //_postprocess_FrameBuffer->attachDepthTexture();
    _postprocess_Texture = _postprocess_FrameBuffer->attachColorTexture();

    if (!_postprocess_FrameBuffer->readyForRender())
        debug::log::println_err ("failed to initialize frame buffer for postprocess");

    auto postprocessShaderId = gpu_program::id (elementary_shapes::simple_vertex_layout::alloc(),
                                               "screen_quad.vert", "screen_quad.frag");
    auto postprocessShader = _resources.gpuProgramsManager().request (postprocessShaderId, _resources);
    auto postprocessTechnique = technique::alloc (postprocessShader);
    postprocessTechnique->transformNotNeeded();
    _postprocessMaterial = material::alloc (postprocessTechnique);
    _postprocessMaterial->textures()["uScreen"] = _postprocess_Texture;
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
    _renderer.renderScene (_scene);

    _renderer.stopForcingMaterial();

    // ---------------------------------------------------------------------------------------------  Render pass

    _renderer.renderTo (_sceneWithoutWater_FrameBuffer);

    auto beforeDrawLambda = [this] (graphics_renderer &renderer){
        object2screen_transform_d shadowmapTranfrom (
                renderer.state().getObject2ScreenTransform().getWorldTransform(),
                _lightTransform.inversed(), _shadowmapCamera->getProjection());
        matrix_4x4_f matBias (0.5f, 0.5f, 0.5f, 1.0f);
        matBias.setCol3 (3, 0.5f, 0.5f, 0.5f);
        auto matShadowmapTransform = shadowmapTranfrom.asMatrix().convertType<float>();
        matBias.multiply (matShadowmapTransform);

        //renderer.state().getMaterial()->textures()["uShadowMapFlat"] = _shadowmapTexture;
        renderer.state().getMaterial()->textures()["uShadowMap"] = _shadowmapTexture;
        renderer.state().getMaterial()->setup (renderer);

        renderer.state().getRenderingProgram()->setUniform ("uShadowmapTransform", matBias, true);
    };

    auto handlerId = _renderer.beforeDrawCallEvent().handleWith (beforeDrawLambda);

    _renderer.use (_viewerCamera);
    _renderer.renderScene (_scene);

    // ---------------------------------------------------------------------------------------------  Reflections

    _waterObject->drawReflections (_renderer, *_scene);
    _renderer.beforeDrawCallEvent().stopHandlingWith (handlerId);

    // ---------------------------------------------------------------------------------------------  Draw water now

    _renderer.renderTo (_postprocess_FrameBuffer, false);

    glDisable (GL_DEPTH_TEST);
    glDepthMask (GL_FALSE);

    auto invProjMat = _viewerCamera->getProjection()->asInverseMatrix();
    _drawScreenMaterial->getTechnique()->getRenderingProgram()->setUniform ("uMatInvProjection",
                                                                            invProjMat.convertType<float>());
    _screenQuad->draw (_renderer);

    glEnable (GL_DEPTH_TEST);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    _renderer.use (_viewerCamera);
    _waterObject->draw (_renderer);
    glDisable (GL_BLEND);

    glDepthMask (GL_TRUE);

    // ---------------------------------------------------------------------------------------------  Finally draw to screen

    _renderer.renderTo (_renderWindow);
    _renderer.forceMaterial (_postprocessMaterial);
    _screenQuad->draw (_renderer);
    _renderer.stopForcingMaterial();
}


void demo_scene::_justTestDraw()
{
    _renderer.renderTo (_shadowmapFrameBuffer);
    _renderer.forceMaterial (_shadowmapGenMaterial);

    _renderer.use (_shadowmapCamera);
    _renderer.renderScene (_scene);

    _renderer.stopForcingMaterial();

    // ---------------------------------------------------------------------------------------------  Render pass

    _renderer.renderTo (_renderWindow);

    auto beforeDrawLambda = [this] (graphics_renderer &renderer){
        object2screen_transform_d shadowmapTranfrom (
                renderer.state().getObject2ScreenTransform().getWorldTransform(),
                _lightTransform.inversed(), _shadowmapCamera->getProjection());
        matrix_4x4_f matBias (0.5f, 0.5f, 0.5f, 1.0f);
        matBias.setCol3 (3, 0.5f, 0.5f, 0.5f);
        auto matShadowmapTransform = shadowmapTranfrom.asMatrix().convertType<float>();
        matBias.multiply (matShadowmapTransform);

        //renderer.state().getMaterial()->textures()["uShadowMapFlat"] = _shadowmapTexture;
        renderer.state().getMaterial()->textures()["uShadowMap"] = _shadowmapTexture;
        renderer.state().getMaterial()->setup (renderer);

        renderer.state().getRenderingProgram()->setUniform ("uShadowmapTransform", matBias, true);
    };

    auto handlerId = _renderer.beforeDrawCallEvent().handleWith (beforeDrawLambda);

    _renderer.use (_viewerCamera);
    _renderer.renderScene (_scene);

    _renderer.beforeDrawCallEvent().stopHandlingWith (handlerId);
}
