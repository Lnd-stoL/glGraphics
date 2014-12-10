
#ifndef __graphics_renderer__included__
#define __graphics_renderer__included__

//----------------------------------------------------------------------------------------------------------------------

#include "renderable.hpp"
#include "camera.hpp"
#include "render_window.hpp"
#include "material.hpp"
#include "frame_buffer.hpp"
#include "scene.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace render
{


//----------------------------------------------------------------------------------------------------------------------

    class graphics_renderer
    {
    public:
        class renderer_state
        {
            math3d::object2screen_transform_d  _object2ScreenTransform;
            camera::ptr  _camera;
            material::ptr  _material;
            frame_buffer::ptr  _frameBuffer;

            bool  _blend           = false;
            bool  _testDepth       = false;
            bool  _backfaceCulling = false;


        public:
            property_get_ref (object2ScreenTransform, _object2ScreenTransform)
            property_get (activeCamera, _camera)
            property_get (activeMaterial, _material)
            property_get (activeRenderingProgram, _material->renderingTechnique()->gpuProgram())
            property_get (activeFrameBuffer, _frameBuffer)

            property_get (blendingEnabled, _blend)
            property_get (testDepth, _testDepth)
            property_get (backfaceCulling, _backfaceCulling)


        public:
            friend class graphics_renderer;
        };

//----------------------------------------------------------------------------------------------------------------------

        class frame_statistics
        {
            unsigned  _drawCalls = 0;
            unsigned  _trianglesCount = 0;

        public:
            property_get (drawCalls, _drawCalls)
            property_get (triangleCount, _trianglesCount)

        public:
            friend class graphics_renderer;
        };

//----------------------------------------------------------------------------------------------------------------------

    private:
        renderer_state _state;
        frame_statistics  _lastFrameStatistics;
        frame_statistics  _frameStatistics;
        scene::ptr _scene;

        static constexpr double _frameCountScale = 0.001;
        float _frameCount = 0;

        bool  _forcedMaterial = false;
        bool  _materialSet = false;

        event<graphics_renderer&>  _beforeDrawCallEvent;


    public:
        property_ref (state, _state)
        property_get_ref (frameStatistics, _frameStatistics)
        property_get_ref (lastFrameStatistics, _lastFrameStatistics)
        property_get (currentScene, _scene)
        property_get (frameCount, _frameCount / _frameCountScale)

        event_access (beforeDrawCall, _beforeDrawCallEvent)


    protected:
        void _setupShaderBeforeDraw();
        inline void _newFrame();

    public:
        declare_ptr_alloc (graphics_renderer)

        graphics_renderer (render_window::ptr renderWindow);

        void use (math3d::object2screen_transform_d &&trans);
        void use (camera::ptr cam);
        void use (material::ptr mat);

        void blend (bool enabled);
        void testDepth (bool enabled);
        void backfaceCulling (bool enabled);

        void renderTo (frame_buffer::ptr frameBuffer, bool autoClear = true);
        void renderTo (render_window &wnd, bool autoClear = true);

        void forceMaterial (material::ptr mat);
        void stopForcingMaterial();

        void draw (gpu_buffer &vertexBuffer, gpu_buffer &indexBuffer, uint8_t bytesPerIndex = 2, unsigned indicesCount = 0);
        void drawPoints (gpu_buffer &vertexBuffer);
        void draw (const renderable &object);

        void renderScene (scene::ptr sceneToRender);
    };

}

//----------------------------------------------------------------------------------------------------------------------

#endif
