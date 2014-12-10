
#include "gpu_image_processing.hpp"
#include "graphics_renderer.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    gpu_image_processing::gpu_image_processing (resources &renderRes, string const &fragShaderName)
    {
        auto shaderId = gpu_program::id (elementary_shapes::simple_vertex_layout::alloc(), "screen_quad.vert", fragShaderName);
        auto shader = renderRes.gpuProgramsManager().request (shaderId, renderRes);
        auto technique = technique::alloc (shader);
        technique->transformNotNeeded();
        material::ptr mat = material::alloc (technique);

        vector<elementary_shapes::simple_vertex> vertices;
        vector<unsigned short> indices;
        elementary_shapes::quadXY (vertices, indices);
        _screenQuad = mesh_component<elementary_shapes::simple_vertex, unsigned short>::alloc (mat, vertices, indices);
        _screenQuad->backfaceCulling (false);
    }


    void gpu_image_processing::input (const string &name, texture::ptr inTxt)
    {
        _screenQuad->componentMaterial()->set (name, inTxt);
    }

//----------------------------------------------------------------------------------------------------------------------

    offscreen_render_target::offscreen_render_target (vector2<unsigned> targetBufferSize,
                                                      unsigned colorBuffersNum, bool depthBuffer)
    {
        _frameBuffer = frame_buffer::alloc (targetBufferSize.x(), targetBufferSize.y());

        _colorBufferTextures.reserve (colorBuffersNum);
        for (unsigned i = 0; i < colorBuffersNum; ++i)
            _colorBufferTextures.emplace_back (_frameBuffer->attachColorTexture());

        if (depthBuffer)  _depthBufferTexture = _frameBuffer->attachDepthTexture();

        if (!_frameBuffer->readyForRender())
            debug::log::println_err ("frame buffer is incomplete");
    }


    offscreen_render_target::offscreen_render_target (vector2<unsigned> targetBufferSize,
                                                         unsigned colorBuffersNum, texture::ptr depthBuffer)
        : offscreen_render_target (targetBufferSize, colorBuffersNum, false)
    {
        _depthBufferTexture = depthBuffer;
        _frameBuffer->attachDepthTexture (depthBuffer);
        _clearFB = false;

        if (!_frameBuffer->readyForRender())
            debug::log::println_err ("frame buffer is incomplete");
    }


    void offscreen_render_target::draw (graphics_renderer &renderer, const renderable& object)
    {
        this->setup (renderer);
        object.draw (renderer);
    }


    void offscreen_render_target::setup (graphics_renderer &renderer)
    {
        renderer.renderTo (_frameBuffer, _clearFB);
    }


    texture::ptr offscreen_render_target::colorTexture()
    {
        if (_colorBufferTextures.size() >= 1)  return _colorBufferTextures[0];
        else  return texture::ptr();
    }

//----------------------------------------------------------------------------------------------------------------------

    gpu_image_processing_stage::gpu_image_processing_stage (resources &renderRes, const string &fragShaderName,
                                                            offscreen_render_target::ptr target)
        : gpu_image_processing (renderRes, fragShaderName)
    {
        _target = target;
    }


    void gpu_image_processing_stage::processUsing (graphics_renderer &renderer)
    {
        _target->setup (renderer);

        renderer.testDepth (false);
        _screenQuad->draw (renderer);
        renderer.testDepth (true);
    }


//----------------------------------------------------------------------------------------------------------------------

    gpu_image_processing_screen::gpu_image_processing_screen (resources &renderRes, const string &fragShaderName,
                                                    render_window::ptr target)
        : gpu_image_processing (renderRes, fragShaderName)
    {
        _renderWindow = target;
    }


    void gpu_image_processing_screen::processUsing (graphics_renderer &renderer)
    {
        renderer.renderTo (*(_renderWindow), false);

        renderer.testDepth (false);
        _screenQuad->draw (renderer);
        renderer.testDepth (true);
    }
}
