
#include "shadowmap_shadows.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    shadowmap_shadows::shadowmap_shadows (resources &renderRes, vector2<unsigned> mapSize)
    {
        _shadowmapRT = offscreen_render_target::alloc (mapSize, 0, true);
        _shadowmapRT->depthTexture()->setupForShadowSampler();

        unique_ptr<orthographic_projection_d> lightProj (
                new orthographic_projection_d (180, ((double) mapSize.x()) / mapSize.y(), interval_d (30, 250)));

        _lightCamera = render::camera::alloc (std::move (lightProj));

        auto vertexLayout = shadowmapgen_vertex_layout::alloc();
        auto shadowmapGenProgramId = gpu_program::id (vertexLayout, "shadowmaps/shadowmap_generate.vert",
                                                      "shadowmaps/shadowmap_generate.frag");
        auto shadowmapGenProgram = renderRes.gpuProgramsManager().request (shadowmapGenProgramId, renderRes);
        _shadowmapGenMaterial = material::alloc (technique::alloc (shadowmapGenProgram));

        _matBias = matrix_4x4_f (0.5f, 0.5f, 0.5f, 1.0f);
        _matBias.setCol3 (3, 0.5f, 0.5f, 0.5f);
    }


    void shadowmap_shadows::generateShadowmap (graphics_renderer &renderer, scene::ptr fromScene,
                                               scene::render_group_id shadowCasters)
    {
        renderer.forceMaterial (_shadowmapGenMaterial);

        _shadowmapRT->setup (renderer);
        renderer.use (_lightCamera);
        renderer.testDepth (true);
        renderer.renderScene (fromScene, shadowCasters);

        renderer.stopForcingMaterial();
    }


    void shadowmap_shadows::drawShadedScene (graphics_renderer &renderer, scene::ptr fromScene,
            scene::render_group_id shadowRecv)
    {
        auto beforeDrawLambda = [this] (graphics_renderer &renderer) {
            auto &rendererState = renderer.state();

            object2screen_transform_d shadowmapTranfrom (
                    rendererState.object2ScreenTransform().worldTransform(),
                    _lightCamera->inversedTransform(), _lightCamera->projection());

            matrix_4x4_f biasedShadowmapTrasform = _matBias;
            biasedShadowmapTrasform.multiply (shadowmapTranfrom.asMatrix().convertType<float>());

            rendererState.activeMaterial()->set ("uTxtShadowMap", _shadowmapRT->depthTexture());
            rendererState.activeMaterial()->set ("uMatShadowmapTransform", biasedShadowmapTrasform);
            rendererState.activeMaterial()->setup (renderer);
        };

        auto handlerId = renderer.beforeDrawCallEvent().handleWith (beforeDrawLambda);
        renderer.renderScene (fromScene, shadowRecv);
        renderer.beforeDrawCallEvent().stopHandlingWith (handlerId);
    }


    void shadowmap_shadows::changeLight (const vector3_f &position, const rotation_d &rotation)
    {
        _lightCamera->changeTransform (position.convertType<double>(), rotation);
    }
}
