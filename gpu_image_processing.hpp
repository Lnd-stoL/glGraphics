#ifndef __gpu_image_effect__included__
#define __gpu_image_effect__included__

//----------------------------------------------------------------------------------------------------------------------

#include "math3D.hpp"
#include "renderable.hpp"
#include "frame_buffer.hpp"
#include "mesh.hpp"
#include "render_resources.hpp"
#include "render_window.hpp"

using math3d::vector2;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class gpu_image_processing
    {
    protected:
        mesh_component<elementary_shapes::simple_vertex, unsigned short>::ptr  _screenQuad;

    public:
        property_get (program, _screenQuad->componentMaterial()->renderingTechnique()->gpuProgram())


    public:
        declare_ptr_alloc (gpu_image_processing)
        gpu_image_processing (resources &renderRes, const string &fragShaderName);

        void input (const string& name, texture::ptr inTxt);
        virtual void processUsing (graphics_renderer &renderer) = 0;
    };

//----------------------------------------------------------------------------------------------------------------------

    class offscreen_render_target
    {
    protected:
        frame_buffer::ptr  _frameBuffer;
        bool  _clearFB = true;

        vector<texture::ptr>  _colorBufferTextures;
        texture::ptr  _depthBufferTexture;


    public:
        property_get_ref (colorTextures, _colorBufferTextures)

        property_get (depthTexture, _depthBufferTexture)
        property_get (frameBuffer, _frameBuffer)


    public:
        declare_ptr_alloc (offscreen_render_target)

        offscreen_render_target (vector2<unsigned> targetBufferSize, unsigned colorBuffersNum, bool depthBuffer = false);
        offscreen_render_target (vector2<unsigned> targetBufferSize, unsigned colorBuffersNum, texture::ptr depthBuffer);

        void setup (graphics_renderer &renderer);
        void draw (graphics_renderer &renderer, const renderable& object);

        texture::ptr  colorTexture();
    };

//----------------------------------------------------------------------------------------------------------------------

    class gpu_image_processing_stage :
        public gpu_image_processing
    {
    private:
        offscreen_render_target::ptr  _target;

    public:
        property_get (renderTarget, _target)


    public:
        declare_ptr_alloc (gpu_image_processing_stage)
        gpu_image_processing_stage (resources &renderRes, const string &fragShaderName, offscreen_render_target::ptr target);

        void processUsing (graphics_renderer &renderer);
    };

//----------------------------------------------------------------------------------------------------------------------

    class gpu_image_processing_screen :
        public gpu_image_processing
    {
    private:
        render_window::ptr  _renderWindow;


    public:
        declare_ptr_alloc (gpu_image_processing_screen)
        gpu_image_processing_screen (resources &renderRes, const string &fragShaderName, render_window::ptr target);

        void processUsing (graphics_renderer &renderer);
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
